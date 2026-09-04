import { TelemetryData, ConnectionState } from '../types';

export const BLE_CONFIG = {
  SERVICE_UUID: '1c7d24e0-32a1-4355-8e79-5e72d24260aa',
  TELEMETRY_CHAR_UUID: '1c7d24e1-32a1-4355-8e79-5e72d24260aa',
  CONTROL_CHAR_UUID: '1c7d24e2-32a1-4355-8e79-5e72d24260aa',
};

type TelemetryCallback = (data: TelemetryData) => void;
type StateCallback = (state: ConnectionState, error?: string) => void;

class BleManager {
  private device: any = null;
  private server: any = null;
  private controlChar: any = null;
  private onTelemetryCallback: TelemetryCallback | null = null;
  private onStateCallback: StateCallback | null = null;
  private mockInterval: any = null;
  private mockState: TelemetryData = {
    timestamp: 0,
    pm25_ambient: 135,
    pm10_ambient: 185,
    pm25_outlet: 6,
    pm10_outlet: 8,
    aqi: 192,
    bucket: 3,
    duty: 75,
    battery: 82,
    mode: 1, // Auto
    status: 0b10000111,
    filtrationEfficiency: 95.5,
  };

  public setCallbacks(onTelemetry: TelemetryCallback, onState: StateCallback) {
    this.onTelemetryCallback = onTelemetry;
    this.onStateCallback = onState;
  }

  public isWebBluetoothSupported(): boolean {
    return typeof window !== 'undefined' && typeof navigator !== 'undefined' && 'bluetooth' in navigator;
  }

  public async connect(): Promise<void> {
    if (!this.isWebBluetoothSupported()) {
      if (this.onStateCallback) {
        this.onStateCallback('disconnected', 'Web Bluetooth is not supported in this browser. Use Chrome/Edge on Desktop or Android, or toggle Demo Mode.');
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

      if (this.onStateCallback) this.onStateCallback('connected');
    } catch (err: any) {
      console.error('BLE connection failed:', err);
      if (this.onStateCallback) {
        this.onStateCallback('disconnected', err.message || 'Bluetooth connection failed');
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
    this.disconnect();
    if (this.onStateCallback) this.onStateCallback('mock');

    let tick = 0;
    this.mockInterval = setInterval(() => {
      tick++;
      // Simulate realistic commuter exposure fluctuations
      const noise = (Math.sin(tick * 0.2) * 25) + (Math.random() * 12 - 6);
      const amb25 = Math.max(15, Math.round(140 + noise));
      const amb10 = Math.round(amb25 * 1.4);

      // HEPA H13 cartridge reduces by ~95-97%
      const efficiency = 95.0 + (Math.random() * 2.0);
      const out25 = Math.max(2, Math.round(amb25 * (1 - efficiency / 100)));
      const out10 = Math.max(3, Math.round(amb10 * (1 - efficiency / 100)));

      // Auto duty adapts to AQI
      let duty = this.mockState.duty;
      if (this.mockState.mode === 1) { // Auto
        if (amb25 < 35) duty = 35;
        else if (amb25 < 55) duty = 55;
        else if (amb25 < 150) duty = 75;
        else duty = 80;
      }

      this.mockState = {
        timestamp: tick * 1000,
        pm25_ambient: amb25,
        pm10_ambient: amb10,
        pm25_outlet: out25,
        pm10_outlet: out10,
        aqi: Math.min(500, Math.round(amb25 * 1.35)),
        bucket: amb25 > 150 ? 4 : amb25 > 55 ? 3 : 2,
        duty: duty,
        battery: Math.max(10, 85 - Math.floor(tick / 60)),
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
    if (this.mockInterval) {
      this.mockState.mode = mode;
      if (mode === 0) this.mockState.duty = 0;
      else if (mode === 2) this.mockState.duty = 60;
      if (this.onTelemetryCallback) this.onTelemetryCallback({ ...this.mockState });
      return;
    }

    if (!this.controlChar) return;
    try {
      const data = new Uint8Array([0x01, mode]);
      await this.controlChar.writeValue(data);
    } catch (err) {
      console.error('Failed to set mode:', err);
    }
  }

  public async setDuty(duty: number): Promise<void> {
    if (this.mockInterval) {
      this.mockState.duty = duty;
      this.mockState.mode = 2; // Set to Manual
      if (this.onTelemetryCallback) this.onTelemetryCallback({ ...this.mockState });
      return;
    }

    if (!this.controlChar) return;
    try {
      const data = new Uint8Array([0x02, duty]);
      await this.controlChar.writeValue(data);
    } catch (err) {
      console.error('Failed to set duty:', err);
    }
  }

  public async togglePower(): Promise<void> {
    if (this.mockInterval) {
      this.mockState.mode = this.mockState.mode === 0 ? 1 : 0;
      this.mockState.duty = this.mockState.mode === 0 ? 0 : 50;
      if (this.onTelemetryCallback) this.onTelemetryCallback({ ...this.mockState });
      return;
    }

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
    if (this.onStateCallback) this.onStateCallback('disconnected');
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
