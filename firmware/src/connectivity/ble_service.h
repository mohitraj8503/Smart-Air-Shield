#ifndef SMART_AIR_SHIELD_BLE_SERVICE_H
#define SMART_AIR_SHIELD_BLE_SERVICE_H

#include <stdint.h>
#include "../config.h"

// 17-byte packed binary telemetry frame pushed over BLE notify
struct __attribute__((packed)) TelemetryPacket {
    uint32_t timestamp;        // ESP32 uptime (ms)
    uint16_t pm2_5_ambient;    // Ambient PM2.5 (ug/m3)
    uint16_t pm10_ambient;     // Ambient PM10 (ug/m3)
    uint16_t pm2_5_outlet;     // Clean delivered PM2.5 (ug/m3)
    uint16_t pm10_outlet;      // Clean delivered PM10 (ug/m3)
    uint16_t aqi_value;        // US EPA AQI (0 - 500)
    uint8_t  aqi_bucket;       // AQI Bucket (0 - 5)
    uint8_t  blower_duty;      // Fan duty cycle (0 - 100%)
    uint8_t  battery_pct;      // Battery state of charge (0 - 100%)
    uint8_t  operating_mode;   // 0: Off, 1: Auto, 2: Manual
    uint8_t  status_flags;     // Status/fault bitmask
};

enum BleCommandType {
    CMD_SET_MODE = 0x01,       // [0x01, mode (0: Off, 1: Auto, 2: Manual)]
    CMD_SET_DUTY = 0x02,       // [0x02, duty_pct (0 - 100)]
    CMD_POWER_TOGGLE = 0x03    // [0x03]
};

// Callback prototype for dashboard write commands
typedef void (*BleCommandCallback)(BleCommandType cmd, uint8_t value);

class BleServiceManager {
public:
    BleServiceManager();

    bool begin(BleCommandCallback cmdCallback = nullptr);
    void updateTelemetry(const TelemetryPacket& packet);
    bool isConnected() const;

private:
    bool _initialized;
    BleCommandCallback _cmdCallback;
};

#endif // SMART_AIR_SHIELD_BLE_SERVICE_H
