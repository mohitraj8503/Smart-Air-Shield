/**
 * Adapted from unloquer/esp32-pms7003 (src/status.h)
 * Enhanced with 8-bit status/fault flags, LED cadence sequencer & non-blocking buzzer tones.
 */

#ifndef SMART_AIR_SHIELD_LED_BUZZER_H
#define SMART_AIR_SHIELD_LED_BUZZER_H

#include <stdint.h>
#include <bitset>

// System Status & Fault Bit Positions (std::bitset<8>)
enum StatusBit {
    STATUS_BIT_AMB_SENSOR_OK = 0,   // Ambient sensor communicating
    STATUS_BIT_OUT_SENSOR_OK = 1,   // Outlet sensor communicating
    STATUS_BIT_BLE_CONNECTED = 2,   // BLE client paired
    STATUS_BIT_WIFI_ACTIVE   = 3,   // WiFi AP / logging active
    STATUS_BIT_LOW_BATTERY   = 4,   // Battery < 10%
    STATUS_BIT_FAULT_WATCHDOG= 5,   // Sensor timeout > 5s
    STATUS_BIT_MANUAL_MODE   = 6,   // Manual blower override
    STATUS_BIT_POWER_ON      = 7    // System powered on
};

class IndicatorController {
public:
    IndicatorController();

    void begin(int ledPin, int buzzerPin);

    // Set / Clear status bits
    void setBit(StatusBit bit, bool value);
    bool getBit(StatusBit bit) const;
    uint8_t getStatusByte() const;

    // Trigger audible notifications
    void playPowerOnTone();
    void playPowerOffTone();
    void playModeChangeBeep();
    void playAlertBeep();

    // Main non-blocking update tick (called in UI task)
    void update(uint32_t nowMs);

private:
    int _ledPin;
    int _buzzerPin;
    std::bitset<8> _status;

    // Buzzer state machine
    uint32_t _buzzerEndTime;
    bool _buzzerActive;

    // Periodic low-battery alert timer
    uint32_t _lastLowBatteryChirp;

    // LED blink sequencer
    uint32_t _lastLedToggle;
    bool _ledState;
};

#endif // SMART_AIR_SHIELD_LED_BUZZER_H
