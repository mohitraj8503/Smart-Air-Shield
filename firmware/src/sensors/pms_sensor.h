/**
 * Adapted from TitaniumMonkey/ESP32-Air-Quality-Sensor (src/sensors/pms7003_sensor.cpp)
 * Enhanced for dual-sensor comparative monitoring with full checksum validation & instantiable channels.
 */

#ifndef SMART_AIR_SHIELD_PMS_SENSOR_H
#define SMART_AIR_SHIELD_PMS_SENSOR_H

#include <stdint.h>
#include <stddef.h>

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#include <HardwareSerial.h>
#endif

// PM sensor data representation
struct PMReading {
    uint16_t pm1_0;      // Standard particle PM1.0 (ug/m3)
    uint16_t pm2_5;      // Standard particle PM2.5 (ug/m3)
    uint16_t pm10;       // Standard particle PM10 (ug/m3)
    uint16_t pm1_0_atm;  // Atmospheric PM1.0 (ug/m3)
    uint16_t pm2_5_atm;  // Atmospheric PM2.5 (ug/m3)
    uint16_t pm10_atm;   // Atmospheric PM10 (ug/m3)
    uint32_t timestamp;  // Milliseconds tick when read
    bool valid;          // True if valid checksum and warm-up elapsed
};

class PMS7003Sensor {
public:
    PMS7003Sensor();

    // Verify 32-byte PMS frame checksum (Sum of bytes 0-29 == (byte 30 << 8) | byte 31)
    static bool verifyChecksum(const uint8_t* buffer, size_t length = 32);

    // Parse a completed 32-byte buffer into a PMReading
    static bool parseBuffer(const uint8_t* buffer, PMReading& reading, uint32_t nowMs = 0);

    // Byte-by-byte frame parser state machine (for streaming data or testing)
    bool processByte(uint8_t byte, uint32_t nowMs = 0);

#ifndef NATIVE_UNIT_TEST
    // Hardware setup
    void begin(HardwareSerial* serialPort, int rxPin, int txPin, uint32_t baud = 9600);

    // Non-blocking poll: reads available serial bytes, returns true when a new frame is decoded
    bool update(uint32_t nowMs);
#endif

    const PMReading& getLatestReading() const { return _latestReading; }
    bool isWarmedUp(uint32_t nowMs) const;
    uint32_t getLastValidFrameTime() const { return _lastValidFrameTime; }
    bool isTimedOut(uint32_t nowMs, uint32_t timeoutMs = 5000) const;

    void reset();

private:
    uint8_t _buffer[32];
    size_t _bufferIndex;
    PMReading _latestReading;
    uint32_t _startTimeMs;
    uint32_t _lastValidFrameTime;
    bool _hasValidFrame;

#ifndef NATIVE_UNIT_TEST
    HardwareSerial* _serial;
#endif
};

#endif // SMART_AIR_SHIELD_PMS_SENSOR_H
