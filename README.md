# SMART AIR-SHIELD

> **Removable Air-Purification & Monitoring Module for Two-Wheeler Helmets**  
> *Submitted for Vishwakarma Awards 2026–27*  
> **Theme:** Sustainable Cities / Smart Mobility &bull; **Sub-theme:** Clean Water, Sanitation & Air Quality Monitoring  
> **Mentor Track:** IIT Hyderabad Track

---

## 1. Project Overview

**SMART AIR-SHIELD** is a compact, removable, non-structural helmet attachment designed to protect two-wheeler commuters from urban particulate air pollution ($PM_{2.5}$ and $PM_{10}$). The device integrates hyperlocal ambient particulate sensing, 3-stage HEPA air filtration, an adaptively-controlled centrifugal blower that delivers filtered air directly to the rider's breathing zone, an onboard $0.96"$ OLED readout, and a real-time **Next.js** companion dashboard over Bluetooth Low Energy (BLE).

Unlike passive respirators or conventional impact-only helmets, SMART AIR-SHIELD actively monitors external roadside pollution, dynamically adapts airflow to particulate concentrations, and verifies breathing-zone air delivery using an outlet sensor to demonstrate real-time particulate reduction.

---

## 2. Multi-Disciplinary Team Roles

| Role | Discipline | Responsibilities |
| :--- | :--- | :--- |
| **Student 1** | Mechanical Engineering | 3D CAD modeling, 3-stage filter bay layout, helmet rim clamp bracket, aerodynamic duct routing, rapid prototyping (PETG/ABS). |
| **Student 2** | Electrical & Electronics (ECE/EEE) | Sensor power rails, 7.4V 2S Li-ion battery & BMS circuitry, N-MOSFET PWM blower driver, voltage dividers, wiring harness assembly. |
| **Student 3** | Computer Science / ECE | ESP32 FreeRTOS firmware, dual PMS7003 UART drivers, EPA AQI control laws, NimBLE GATT telemetry, Next.js companion dashboard. |

---

## 3. Key Innovations & Competitive Advantage

1. **Dual-Sensor Comparative Verification:** Unlike single-sensor monitors, SMART AIR-SHIELD features paired inlet and outlet laser sensors, providing empirical proof of particulate reduction ($\ge 95\%$ HEPA H13 target) live on the dashboard.
2. **Adaptive Slew-Rate Blower Regulation:** PWM control dynamically scales airflow ($2 - 8\text{ L/min}$) across US EPA AQI buckets while enforcing a $\pm 5\%$ per tick slew rate to eliminate motor jerk and keep acoustics $< 35\text{ dB(A)}$ at 1 meter.
3. **Non-Structural, Zero-Modification Helmet Mounting:** Fastens via an external rubber-lined rim clamp. Complies strictly with safety rules: **no drilling, cutting, or adhesive alteration of the protective helmet shell**.
4. **Offline Resilience & Open Telemetry:** Functions completely autonomously with onboard OLED and local LittleFS flash logging, while exposing Web Bluetooth GATT and local WiFi HTTP endpoints (`/log.csv`) for zero-friction data export.

---

## 4. Repository Structure

```text
Project Vishwakarma/
├── README.md                      # Comprehensive project guide, wiring & setup
├── LICENSE                        # MIT License
├── .gitignore                     # Git ignore rules for PlatformIO, Node, and Next.js
├── firmware/                      # ESP32 Embedded Firmware (PlatformIO)
│   ├── platformio.ini             # Target environments: esp32dev (firmware) & native (tests)
│   ├── src/
│   │   ├── main.cpp               # FreeRTOS multitasking, task dispatch, setup/loop
│   │   ├── config.h               # Centralized GPIO pinout, tunables, and BLE UUIDs
│   │   ├── sensors/
│   │   │   ├── pms_sensor.h/.cpp  # Dual PMS7003 UART driver + 32-byte checksum validator
│   │   │   └── battery.h/.cpp     # 2S Li-ion ADC reader + non-linear SoC lookup curve
│   │   ├── control/
│   │   │   ├── aqi.h/.cpp         # US EPA PM2.5 piecewise linear AQI interpolation
│   │   │   └── blower_control.h/.cpp # Adaptive duty control law + slew-rate limiter
│   │   ├── ui/
│   │   │   ├── display.h/.cpp     # 0.96" SSD1306 OLED rendering using U8g2
│   │   │   ├── buttons.h/.cpp     # Debounced tactile button state machine (Mode/Power)
│   │   │   └── led_buzzer.h/.cpp  # Status bitset, LED sequences, and non-blocking alert tones
│   │   ├── connectivity/
│   │   │   ├── ble_service.h/.cpp # NimBLE GATT server: 17-byte telemetry notify + control write
│   │   │   └── wifi_logger.h/.cpp # SoftAP & embedded HTTP server (/status, /log.csv)
│   │   └── storage/
│   │       └── session_log.h/.cpp # Circular RAM buffer + LittleFS flash CSV log
│   ├── test/                      # PlatformIO Native Unit Tests (No hardware required)
│   │   ├── test_aqi/              # Validates EPA breakpoint boundaries and interpolation
│   │   ├── test_control/          # Validates adaptive mapping, slew limiting, and safety clamps
│   │   └── test_pms/              # Validates PMS7003 checksums, corruptions, and frame sync
│   └── docs/
│       └── wiring_diagram.md      # Pinout table, Mermaid diagram, and circuit schematics
├── dashboard/                     # Next.js Companion App (App Router + Tailwind + Web BLE)
│   ├── package.json               # Next.js 14, React 18, Tailwind CSS, Lucide icons
│   ├── next.config.mjs            # Next.js configuration
│   ├── tsconfig.json              # TypeScript configuration
│   ├── src/
│   │   ├── app/                   # Next.js App Router (layout.tsx, page.tsx, globals.css)
│   │   ├── components/            # LiveAQI, FanControl, BatteryIndicator, SessionHistory
│   │   ├── ble/                   # Web Bluetooth API client + built-in demo simulator
│   │   └── types/                 # TypeScript interfaces for telemetry and states
│   └── public/                    # Static web assets
├── hardware/                      # Mechanical & Hardware Deliverables
│   ├── bom.csv                    # Complete Bill of Materials with part numbers & pricing
│   └── enclosure/
│       └── README.md              # Enclosure dimensions, CAD guidelines, mounting constraints
├── docs/                          # Project Documentation
│   ├── SYSTEM_ARCHITECTURE.md     # Architecture diagrams, GATT byte layout, task priorities
│   ├── TEST_PLAN.md               # Airflow bench tests, particle challenge, on-road protocol
│   └── SAFETY.md                  # Verbatim safety non-claims and non-structural rules
└── .github/
    └── workflows/
        └── ci.yml                 # Automated CI: ESP32 build, native tests, Next.js build
```

---

## 5. Hardware Specifications & Bill of Materials

| Subsystem | Component | Interface | Key Specification |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | ESP32-WROOM-32D | Dual Core | 240 MHz, 4MB Flash, integrated WiFi & BLE |
| **Ambient PM Sensor** | Plantower PMS7003 | UART1 (GPIO 16/17) | Laser scattering $0.3 - 10\,\mu\text{m}$, range $0 - 1000\,\mu\text{g}/\text{m}^3$ |
| **Outlet PM Sensor** | Plantower PMS7003 | UART2 (GPIO 25/26) | Identical sensor mounted at breathing zone delivery duct |
| **Blower Driver** | Centrifugal Blower 5015 | PWM (GPIO 27) | $25\text{ kHz}$ ultrasonic PWM via N-Channel MOSFET |
| **Display** | 0.96" SSD1306 OLED | I2C (GPIO 21/22) | $128 \times 64$ monochrome display ($2\text{ Hz}$ refresh) |
| **Battery & BMS** | 7.4V 2S Li-ion 2500mAh | ADC (GPIO 34) | $6.0\text{V} - 8.4\text{V}$ with integrated BMS, $6 - 8\text{h}$ runtime |
| **User Controls** | 2x Tactile Buttons | GPIO 32 / 33 | Debounced Mode selector & Power switch |
| **Indicators** | LED & Piezo Buzzer | GPIO 2 / 15 | System status LED and audible low-battery warning |

*For complete itemized costs ($\approx ₹10,415$ total prototype cost), see [`hardware/bom.csv`](hardware/bom.csv).*  
*For schematics and pin-to-pin connections, see [`firmware/docs/wiring_diagram.md`](firmware/docs/wiring_diagram.md).*

---

## 6. Quickstart Guide

### 6.1 Firmware Development & Flashing (PlatformIO)

#### Prerequisites
Install [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) or PlatformIO IDE:
```bash
pip install platformio
```

#### Run Native Unit Tests (No ESP32 Hardware Required)
Run the automated test suite locally on your computer to verify EPA AQI breakpoints, blower slew rates, and PMS7003 checksum validation:
```bash
cd firmware
pio test -e native
```

#### Build & Flash ESP32 Firmware
Connect your ESP32 development board via USB and run:
```bash
cd firmware
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

---

### 6.2 Next.js Companion Dashboard

The companion app is built with **Next.js 14 (App Router)**, **TypeScript**, and **Tailwind CSS**. It communicates directly with the ESP32 using the standard **Web Bluetooth API**.

#### Run Dashboard Locally
```bash
cd dashboard
npm install
npm run dev
```
Open [http://localhost:3000](http://localhost:3000) in **Google Chrome** or **Microsoft Edge**.

#### Bluetooth Pairing & Demo Mode
- **With Hardware:** Click **Pair Helmet**, select `SMART-AIR-SHIELD` from the Bluetooth prompt. The dashboard will automatically subscribe to the 17-byte telemetry stream and display real-time ambient vs delivered PM values.
- **Without Hardware / Evaluation:** Click **Demo Mode** to start a realistic commuter simulation with dynamic traffic pollution spikes, adaptive blower response, and live HEPA efficiency calculations.
- **Platform Compatibility:** Web Bluetooth is supported on Google Chrome and Microsoft Edge on Windows, Mac, Linux, and Android. On iOS devices, use the Bluefy browser or run Demo Mode.

---

## 7. Testing & Verification Summary

The codebase has been verified against all acceptance criteria:
- **Native Unit Tests:** 13/13 unit tests pass covering EPA breakpoint equations, blower slew limiting, and binary checksum rejection.
- **ESP32 Firmware Target:** Compiles cleanly for `esp32dev` using PlatformIO with NimBLE-Arduino, U8g2, and LittleFS.
- **Next.js Dashboard:** Client-side Web Bluetooth integration with zero hydration issues and built-in mock simulator.
- **Bench & Road Test Protocols:** Documented in [`docs/TEST_PLAN.md`](docs/TEST_PLAN.md) for bench airflow ($2 - 8\text{ L/min}$), acoustic validation ($< 35\text{ dB}$), and paired ON/OFF commuter road trials.

---

## 8. Safety & Compliance

> [!IMPORTANT]
> 1. This device reduces particulate ($PM_{2.5} / PM_{10}$) exposure; it does not claim to remove CO, $NO_x$, or all VOCs.
> 2. It is not a certified respirator or medical device.
> 3. The attachment must be removable and non-structural — firmware/hardware must never require drilling, cutting, or permanently modifying the helmet shell.
> 4. Must not obstruct the visor, chin strap, retention system, or emergency-release mechanism.
> 5. Riders must always wear a BIS-certified helmet; this device is a supplementary accessory only.

*Read the complete safety charter in [`docs/SAFETY.md`](docs/SAFETY.md).*
