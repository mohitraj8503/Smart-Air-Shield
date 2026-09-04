# SMART AIR-SHIELD — System Architecture & Integration Specification

**Product Name:** SMART AIR-SHIELD  
**Tagline:** Removable Air-Purification & Monitoring Module for Two-Wheeler Helmets  
**Target:** Vishwakarma Awards 2026–27 (Theme: Sustainable Cities / Smart Mobility, Clean Water, Sanitation & Air Quality Monitoring, IIT Hyderabad mentor track)

---

## 1. High-Level Block Diagram

```text
┌────────────────────────────┐
│   Ambient Air Inlet (PM)   │
└─────────────┬──────────────┘
              │
              ▼
┌────────────────────────────┐
│ Ambient PM Sensor (PMS7003)│ ──(UART1: GPIO 16/17)──┐
└────────────────────────────┘                        │
                                                      ▼
┌────────────────────────────┐          ┌───────────────────────────┐
│ Outlet PM Sensor (PMS7003) │ ──(UART2)┤                           │ ──(I2C: 21/22)──► SSD1306 OLED (128x64)
└────────────────────────────┘          │     ESP32 Controller      │
                                        │  (FreeRTOS Multitasking)  │ ──(GPIO 2/15)───► Status LED & Buzzer
┌────────────────────────────┐          │                           │
│ Battery ADC Voltage Divider│ ──(ADC1)─┤                           │ ──(PWM: 27)─────► Blower MOSFET Driver
└────────────────────────────┘          └─────────────┬─────────────┘
                                                      │
                                                      ├─(BLE GATT 1Hz Notify)─► Web PWA Companion App
                                                      └─(WiFi HTTP /log.csv)──► Log Data Download
```

---

## 2. FreeRTOS Task Architecture

The firmware executes 4 concurrent FreeRTOS tasks with isolated duties and mutex-protected state sharing:

| Task Name | Priority | Core | Periodicity | Responsibilities |
| :--- | :---: | :---: | :---: | :--- |
| `vSensorTask` | 3 | Core 1 | Continuous (50ms poll) | Reads UART1 (Ambient PMS7003) and UART2 (Outlet PMS7003), enforces 30s warmup, samples Battery ADC every 5s, detects >5s sensor timeouts. |
| `vControlTask` | 3 | Core 1 | 200 ms tick | Computes EPA AQI bucket, evaluates Auto vs Manual control law, applies $\pm 5\%$ slew rate limiter, writes 25 kHz PWM. |
| `vUITask` | 2 | Core 0 | 20 ms poll | Debounces Mode & Power tactile buttons, drives OLED at 2 Hz, sequences status LED blinks and buzzer chirps. |
| `vTelemetryTask` | 1 | Core 0 | 1000 ms (1 Hz) | Packs 17-byte binary telemetry frame, notifies connected BLE app, logs sample to RAM/LittleFS CSV, handles HTTP server requests. |

---

## 3. BLE GATT Specification

### 3.1 Service & Characteristic UUIDs
- **Service UUID:** `1c7d24e0-32a1-4355-8e79-5e72d24260aa`
- **Telemetry Characteristic (Read, Notify):** `1c7d24e1-32a1-4355-8e79-5e72d24260aa`
- **Control Characteristic (Write, WriteNR):** `1c7d24e2-32a1-4355-8e79-5e72d24260aa`

### 3.2 Telemetry Packet Layout (17 Bytes Packed Little-Endian)

| Byte Offset | Field Name | Data Type | Units / Scale | Description |
| :---: | :--- | :--- | :--- | :--- |
| `0..3` | `timestamp` | `uint32_t` | Milliseconds | ESP32 uptime since boot |
| `4..5` | `pm2_5_ambient` | `uint16_t` | $\mu\text{g}/\text{m}^3$ | Ambient PM2.5 particulate reading |
| `6..7` | `pm10_ambient` | `uint16_t` | $\mu\text{g}/\text{m}^3$ | Ambient PM10 particulate reading |
| `8..9` | `pm2_5_outlet` | `uint16_t` | $\mu\text{g}/\text{m}^3$ | Delivered clean breathing zone PM2.5 |
| `10..11`| `pm10_outlet` | `uint16_t` | $\mu\text{g}/\text{m}^3$ | Delivered clean breathing zone PM10 |
| `12..13`| `aqi_value` | `uint16_t` | Index (0–500) | Calculated US EPA AQI |
| `14` | `aqi_bucket` | `uint8_t` | 0 – 5 | EPA category bucket (0: Good ... 5: Hazardous) |
| `15` | `blower_duty` | `uint8_t` | Percent (0–100) | Actual PWM duty cycle of blower |
| `16` | `battery_pct` | `uint8_t` | Percent (0–100) | Remaining battery charge percentage |
| `17` | `operating_mode`| `uint8_t` | Enum | 0 = OFF, 1 = AUTO, 2 = MANUAL |
| `18` | `status_flags` | `uint8_t` | Bitmask | Bit 0: Amb OK, 1: Out OK, 2: BLE OK, 4: Low Bat, 5: Fault |

### 3.3 Control Characteristic Write Commands

Commands written to `1c7d24e2-32a1-4355-8e79-5e72d24260aa`:
- **Set Mode:** `[0x01, mode]` where `mode`: `0` = Off, `1` = Auto, `2` = Manual.
- **Set Manual Duty Cycle:** `[0x02, duty_pct]` where `duty_pct` is `20` to `100`.
- **Toggle Power State:** `[0x03]`

---

## 4. Adaptive Blower Control Law

The adaptive regulation law balances purification airflow against acoustic noise and battery life:

$$\text{Duty}_{\text{Auto}} = \text{clamp}\Big(\text{AQI\_BUCKET\_TO\_DUTY}[\text{bucket}], \text{Min}=20\%, \text{Max}=80\%\Big)$$

$$\text{Duty}_{t} = \text{Duty}_{t-1} + \text{clamp}\Big(\text{Duty}_{\text{Target}} - \text{Duty}_{t-1}, -5\%, +5\%\Big)$$

- **Slew-Rate Limiting:** A maximum change of $\pm 5\%$ per 200 ms tick ensures the centrifugal blower transitions smoothly over $\approx 1$ second without electrical transients or audible whines.
- **Acoustic Ceiling:** Clamped at $\le 80\%$ duty cycle in Auto mode to ensure noise remains $< 35\text{ dB}$ at 1 m from the helmet.
- **Sensor Watchdog Fallback:** If either PM sensor fails to deliver a valid checksummed frame for $> 5$ seconds, the controller transitions to a safe fallback duty cycle ($50\%$) and illuminates the fault icon on the OLED and mobile dashboard.

---

## 5. Local WiFi HTTP Export API

When enabled via long-pressing the Power button, the ESP32 acts as a SoftAP (`SMART-AIR-SHIELD`):
- `GET /`: Lightweight mobile-friendly telemetry summary HTML page.
- `GET /status`: JSON endpoint providing real-time sensor and blower values.
- `GET /log.csv`: Directly streams the entire stored ride session history CSV from LittleFS flash storage.
