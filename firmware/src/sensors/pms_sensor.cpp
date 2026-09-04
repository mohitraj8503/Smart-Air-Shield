/**
 * Adapted from TitaniumMonkey/ESP32-Air-Quality-Sensor (src/sensors/pms7003_sensor.cpp)
 * Enhanced for dual-sensor comparative monitoring with full checksum validation & instantiable channels.
 */

#include "pms_sensor.h"
#include "../config.h"

PMS7003Sensor::PMS7003Sensor()
    : _bufferIndex(0),
      _startTimeMs(0),
      _lastValidFrameTime(0),
      _hasValidFrame(false)
#ifndef NATIVE_UNIT_TEST
      , _serial(nullptr)
#endif
{
    _latestReading = {0, 0, 0, 0, 0, 0, 0, false};
    for (size_t i = 0; i < sizeof(_buffer); i++) {
        _buffer[i] = 0;
    }
}

void PMS7003Sensor::reset() {
    _bufferIndex = 0;
    _hasValidFrame = false;
    _latestReading.valid = false;
}

bool PMS7003Sensor::isWarmedUp(uint32_t nowMs) const {
    if (_startTimeMs == 0) {
        return false;
    }
    return (nowMs - _startTimeMs) >= SENSOR_WARMUP_TIME_MS;
}

bool PMS7003Sensor::isTimedOut(uint32_t nowMs, uint32_t timeoutMs) const {
    if (!_hasValidFrame) {
        return (nowMs - _startTimeMs) > timeoutMs;
    }
    return (nowMs - _lastValidFrameTime) > timeoutMs;
}

bool PMS7003Sensor::verifyChecksum(const uint8_t* buffer, size_t length) {
    if (buffer == nullptr || length < 32) {
        return false;
    }
    if (buffer[0] != 0x42 || buffer[1] != 0x4D) {
        return false;
    }

    uint16_t calculatedSum = 0;
    for (size_t i = 0; i < 30; i++) {
        calculatedSum += buffer[i];
    }

    uint16_t expectedChecksum = ((uint16_t)buffer[30] << 8) | buffer[31];
    return calculatedSum == expectedChecksum;
}

bool PMS7003Sensor::parseBuffer(const uint8_t* buffer, PMReading& reading, uint32_t nowMs) {
    if (!verifyChecksum(buffer, 32)) {
        return false;
    }

    reading.pm1_0     = ((uint16_t)buffer[4]  << 8) | buffer[5];
    reading.pm2_5     = ((uint16_t)buffer[6]  << 8) | buffer[7];
    reading.pm10      = ((uint16_t)buffer[8]  << 8) | buffer[9];
    reading.pm1_0_atm = ((uint16_t)buffer[10] << 8) | buffer[11];
    reading.pm2_5_atm = ((uint16_t)buffer[12] << 8) | buffer[13];
    reading.pm10_atm  = ((uint16_t)buffer[14] << 8) | buffer[15];
    reading.timestamp = nowMs;
    reading.valid     = true;

    return true;
}

bool PMS7003Sensor::processByte(uint8_t byte, uint32_t nowMs) {
    if (_startTimeMs == 0) {
        _startTimeMs = nowMs;
    }

    // State 0: Look for preamble byte 0x42
    if (_bufferIndex == 0) {
        if (byte == 0x42) {
            _buffer[0] = byte;
            _bufferIndex = 1;
        }
        return false;
    }

    // State 1: Look for preamble byte 0x4D
    if (_bufferIndex == 1) {
        if (byte == 0x4D) {
            _buffer[1] = byte;
            _bufferIndex = 2;
        } else {
            _bufferIndex = 0;
            if (byte == 0x42) { // Recover if 0x42 was immediately repeated
                _buffer[0] = byte;
                _bufferIndex = 1;
            }
        }
        return false;
    }

    // Accumulate payload bytes
    _buffer[_bufferIndex++] = byte;

    // Full 32-byte frame received
    if (_bufferIndex >= 32) {
        _bufferIndex = 0; // Reset index for next frame

        PMReading parsed;
        if (parseBuffer(_buffer, parsed, nowMs)) {
            _latestReading = parsed;
            _lastValidFrameTime = nowMs;
            _hasValidFrame = true;

            // Only mark reading strictly valid if warm-up period is satisfied
            if (!isWarmedUp(nowMs)) {
                _latestReading.valid = false;
                return false;
            }
            return true;
        }
    }

    return false;
}

#ifndef NATIVE_UNIT_TEST
void PMS7003Sensor::begin(HardwareSerial* serialPort, int rxPin, int txPin, uint32_t baud) {
    _serial = serialPort;
    _startTimeMs = millis();
    if (_serial != nullptr) {
        _serial->begin(baud, SERIAL_8N1, rxPin, txPin);

        // Put PMS7003 into active continuous reporting mode
        uint8_t activeModeCmd[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
        _serial->write(activeModeCmd, sizeof(activeModeCmd));
    }
}

bool PMS7003Sensor::update(uint32_t nowMs) {
    if (_serial == nullptr) {
        return false;
    }

    bool newFrameParsed = false;
    while (_serial->available() > 0) {
        uint8_t b = (uint8_t)_serial->read();
        if (processByte(b, nowMs)) {
            newFrameParsed = true;
        }
    }

    return newFrameParsed;
}
#endif
