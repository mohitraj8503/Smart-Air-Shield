import { TelemetryData, ConnectionState } from '../types';

export const BLE_CONFIG = {
  SERVICE_UUID: '1c7d24e0-32a1-4355-8e79-5e72d24260aa',
  TELEMETRY_CHAR_UUID: '1c7d24e1-32a1-4355-8e79-5e72d24260aa',
  CONTROL_CHAR_UUID: '1c7d24e2-32a1-4355-8e79-5e72d24260aa',
};

type TelemetryCallback = (data: TelemetryData) => void;
type StateCallback = (state: ConnectionState, message?: string) => void;

class BleManager {
  private device: any = null;
  private server: any = null;
  private controlChar: any = null;
  private onTelemetryCallback: TelemetryCallback | null = null;
  private onStateCallback: StateCallback | null = null;
  private mockInterval: any = null;
  private tickCount: number = 0;
  private mockState: TelemetryData = {
    timestamp: 0,
    pm25_ambient: 142,
    pm10_ambient: 188,
    pm25_outlet: 6,
    pm10_outlet: 9,
    aqi: 195,
    bucket: 3,
    duty: 75,
    battery: 84,
    mode: 1, // Auto adaptive
    status: 0b10000111,
    filtrationEfficiency: 95.8,
  };

  public setCallbacks(onTelemetry: TelemetryCallback, onState: StateCallback) {
    this.onTelemetryCallback = onTelemetry;
    this.onStateCallback = onState;

    // Immediately push initial calibrated frame so UI is never blank
    this.onTelemetryCallback({ ...this.mockState });
  }

  public isWebBluetoothSupported(): boolean {
    return typeof window !== 'undefined' && typeof navigator !== 'undefined' && 'bluetooth' in navigator;
  }

  public async connect(): Promise<void> {
    if (!this.isWebBluetoothSupported()) {
      // In Brave or Firefox, gracefully fallback to the live simulated feed with an Apple-style note
      this.startMockMode();
      if (this.onStateCallback) {
        this.onStateCallback('mock', 'Brave Shields / Privacy mode detected. Connected to live simulated telemetry feed.');
      }
      return;
    }

    try {
      if (this.onStateCallback) this.onStateCallback('connecting');

      const navBluetooth = (navigator as any).bluetooth;
      this.device = await navBluetooth.requestDevice({
        filters: [{ namePrefix: 'SMART-AIR-SHIELD' }],
        optionalServices: [BLE_CONFIG.SERVICE_UUID],
      });

      this.device.addEventListener('gattserverdisconnected', this.handleDisconnect.bind(this));

      this.server = await this.device.gatt.connect();
      const service = await this.server.getPrimaryService(BLE_CONFIG.SERVICE_UUID);

      const telemetryChar = await service.getCharacteristic(BLE_CONFIG.TELEMETRY_CHAR_UUID);
      await telemetryChar.startNotifications();
      telemetryChar.addEventListener('characteristicvaluechanged', this.handleTelemetryNotification.bind(this));

      this.controlChar = await service.getCharacteristic(BLE_CONFIG.CONTROL_CHAR_UUID);

      // Stop mock mode if active once real hardware is connected
      if (this.mockInterval) {
        clearInterval(this.mockInterval);
        this.mockInterval = null;
      }

      if (this.onStateCallback) this.onStateCallback('connected');
    } catch (err: any) {
      console.warn('Bluetooth pairing dismissed or unavailable, falling back to simulated stream:', err);
      // Seamlessly keep simulated feed going so user always enjoys a working UI
      this.startMockMode();
      if (this.onStateCallback) {
        this.onStateCallback('mock', 'Live helmet simulation feed active.');
      }
    }
  }

  public disconnect(): void {
    if (this.mockInterval) {
      clearInterval(this.mockInterval);
      this.mockInterval = null;
    }

    if (this.device && this.device.gatt && this.device.gatt.connected) {
      this.device.gatt.disconnect();
    }
    this.handleDisconnect();
  }

  public startMockMode(): void {
    if (this.mockInterval) {
      clearInterval(this.mockInterval);
    }
    if (this.onStateCallback) this.onStateCallback('mock');

    this.mockInterval = setInterval(() => {
      this.tickCount++;
      // Realistic commuter exposure simulation
      // Base traffic wave + random jitter
      const wave = Math.sin(this.tickCount * 0.15) * 28;
      const jitter = (Math.random() * 10 - 5);
      const amb25 = Math.max(18, Math.round(135 + wave + jitter));
      const amb10 = Math.round(amb25 * 1.35);

      // HEPA H13 cartridge achieves 95.0% - 97.5% reduction
      const efficiency = 95.2 + (Math.random() * 1.8);
      const out25 = Math.max(2, Math.round(amb25 * (1 - efficiency / 100)));
      const out10 = Math.max(3, Math.round(amb10 * (1 - efficiency / 100)));

      // Auto mode blower duty calculation
      let duty = this.mockState.duty;
      if (this.mockState.mode === 1) { // Auto
        if (amb25 < 35) duty = 35;
        else if (amb25 < 55) duty = 55;
        else if (amb25 < 150) duty = 75;
        else duty = 80;
      }

      this.mockState = {
        timestamp: this.tickCount * 1000,
        pm25_ambient: amb25,
        pm10_ambient: amb10,
        pm25_outlet: out25,
        pm10_outlet: out10,
        aqi: Math.min(500, Math.round(amb25 * 1.38)),
        bucket: amb25 > 150 ? 4 : amb25 > 55 ? 3 : amb25 > 35 ? 2 : 1,
        duty: duty,
        battery: Math.max(15, 84 - Math.floor(this.tickCount / 80)),
        mode: this.mockState.mode,
        status: 0b10000111,
        filtrationEfficiency: parseFloat(efficiency.toFixed(1)),
      };

      if (this.onTelemetryCallback) {
        this.onTelemetryCallback({ ...this.mockState });
      }
    }, 1000);
  }

  public async setMode(mode: number): Promise<void> {
    this.mockState.mode = mode;
    if (mode === 0) this.mockState.duty = 0;
    else if (mode === 2 && this.mockState.duty === 0) this.mockState.duty = 60;
    if (this.onTelemetryCallback) this.onTelemetryCallback({ ...this.mockState });

    if (!this.controlChar) return;
    try {
      const data = new Uint8Array([0x01, mode]);
      await this.controlChar.writeValue(data);
    } catch (err) {
      console.error('Failed to set mode:', err);
    }
  }

  public async setDuty(duty: number): Promise<void> {
    this.mockState.duty = duty;
    this.mockState.mode = 2; // Switch to Manual
    if (this.onTelemetryCallback) this.onTelemetryCallback({ ...this.mockState });

    if (!this.controlChar) return;
    try {
      const data = new Uint8Array([0x02, duty]);
      await this.controlChar.writeValue(data);
    } catch (err) {
      console.error('Failed to set duty:', err);
    }
  }

  public async togglePower(): Promise<void> {
    const nextMode = this.mockState.mode === 0 ? 1 : 0;
    this.mockState.mode = nextMode;
    this.mockState.duty = nextMode === 0 ? 0 : 50;
    if (this.onTelemetryCallback) this.onTelemetryCallback({ ...this.mockState });

    if (!this.controlChar) return;
    try {
      const data = new Uint8Array([0x03]);
      await this.controlChar.writeValue(data);
    } catch (err) {
      console.error('Failed to toggle power:', err);
    }
  }

  private handleDisconnect(): void {
    this.device = null;
    this.server = null;
    this.controlChar = null;
    // Gracefully restart mock mode on hardware disconnect so display never blanks out
    this.startMockMode();
  }

  private handleTelemetryNotification(event: any): void {
    const value = event.target.value as DataView;
    if (value.byteLength < 17) return;

    const timestamp = value.getUint32(0, true);
    const pm25_ambient = value.getUint16(4, true);
    const pm10_ambient = value.getUint16(6, true);
    const pm25_outlet = value.getUint16(8, true);
    const pm10_outlet = value.getUint16(10, true);
    const aqi = value.getUint16(12, true);
    const bucket = value.getUint8(14);
    const duty = value.getUint8(15);
    const battery = value.getUint8(16);
    const mode = value.byteLength > 17 ? value.getUint8(17) : 1;
    const status = value.byteLength > 18 ? value.getUint8(18) : 0;

    let efficiency = 0;
    if (pm25_ambient > 0) {
      efficiency = Math.max(0, Math.min(100, ((pm25_ambient - pm25_outlet) / pm25_ambient) * 100));
    }

    const data: TelemetryData = {
      timestamp,
      pm25_ambient,
      pm10_ambient,
      pm25_outlet,
      pm10_outlet,
      aqi,
      bucket,
      duty,
      battery,
      mode,
      status,
      filtrationEfficiency: parseFloat(efficiency.toFixed(1)),
    };

    if (this.onTelemetryCallback) {
      this.onTelemetryCallback(data);
    }
  }
}

export const bleManager = new BleManager();
