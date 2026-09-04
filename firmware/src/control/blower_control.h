/**
 * Adapted from Harnas/ESP32_Airpurifier2 (lib/Fan/Fan.cpp & src/AirCleanerController.cpp)
 * Enhanced for dual-sensor comparative monitoring with discrete AQI-bucket table & slew-rate limiting.
 */

#ifndef SMART_AIR_SHIELD_BLOWER_CONTROL_H
#define SMART_AIR_SHIELD_BLOWER_CONTROL_H

#include <stdint.h>
#include "aqi.h"

enum OperatingMode {
    MODE_OFF = 0,
    MODE_AUTO = 1,
    MODE_MANUAL = 2
};

enum ManualPreset {
    MANUAL_PRESET_LOW = 0,
    MANUAL_PRESET_MED = 1,
    MANUAL_PRESET_HIGH = 2,
    MANUAL_PRESET_CUSTOM = 3
};

class BlowerController {
public:
    BlowerController();

    void begin(int pwmPin, int pwmChannel = 0, int freqHz = 25000, int resolutionBits = 10);

    // Main control update tick (recommended every 200 ms)
    // ambientAqiBucket: 0 to 5
    // sensorFault: true if sensor timed out (>5s without frame)
    uint8_t update(int ambientAqiBucket, bool sensorFault = false);

    // Mode selection
    void setMode(OperatingMode mode);
    OperatingMode getMode() const { return _mode; }

    // Manual speed control
    void setManualDuty(uint8_t dutyPct);
    void cycleManualPreset();
    uint8_t getManualDuty() const { return _manualDuty; }

    // State getters
    uint8_t getTargetDuty() const { return _targetDuty; }
    uint8_t getCurrentDuty() const { return _currentDuty; }
    bool isPoweredOn() const { return _mode != MODE_OFF; }

    // Toggle power On (restores last mode) / Off
    void togglePower();

    // Slew rate step calculation (public for unit test verification)
    static uint8_t computeSlewStep(uint8_t current, uint8_t target, uint8_t maxStep);

private:
    void applyPwm(uint8_t dutyPct);

    OperatingMode _mode;
    OperatingMode _lastActiveMode;
    uint8_t _manualDuty;
    ManualPreset _currentPreset;
    uint8_t _targetDuty;
    uint8_t _currentDuty;
    int _pwmPin;
    int _pwmChannel;
    int _pwmMaxTicks;
    bool _initialized;
};

#endif // SMART_AIR_SHIELD_BLOWER_CONTROL_H
