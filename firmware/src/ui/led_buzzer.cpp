/**
 * Adapted from unloquer/esp32-pms7003 (src/status.h)
 * Enhanced with 8-bit status/fault flags, LED cadence sequencer & non-blocking buzzer tones.
 */

#include "led_buzzer.h"
#include "../config.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#endif

IndicatorController::IndicatorController()
    : _ledPin(PIN_STATUS_LED),
      _buzzerPin(PIN_BUZZER),
      _buzzerEndTime(0),
      _buzzerActive(false),
      _lastLowBatteryChirp(0),
      _lastLedToggle(0),
      _ledState(false)
{
    _status.reset();
    _status.set(STATUS_BIT_POWER_ON, true);
}

void IndicatorController::begin(int ledPin, int buzzerPin) {
    _ledPin = ledPin;
    _buzzerPin = buzzerPin;

#ifndef NATIVE_UNIT_TEST
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);

    pinMode(_buzzerPin, OUTPUT);
    digitalWrite(_buzzerPin, LOW);
#endif
}

void IndicatorController::setBit(StatusBit bit, bool value) {
    _status.set(bit, value);
}

bool IndicatorController::getBit(StatusBit bit) const {
    return _status.test(bit);
}

uint8_t IndicatorController::getStatusByte() const {
    return (uint8_t)_status.to_ulong();
}

void IndicatorController::playPowerOnTone() {
#ifndef NATIVE_UNIT_TEST
    digitalWrite(_buzzerPin, HIGH);
    _buzzerEndTime = millis() + 150;
    _buzzerActive = true;
#endif
}

void IndicatorController::playPowerOffTone() {
#ifndef NATIVE_UNIT_TEST
    digitalWrite(_buzzerPin, HIGH);
    _buzzerEndTime = millis() + 300;
    _buzzerActive = true;
#endif
}

void IndicatorController::playModeChangeBeep() {
#ifndef NATIVE_UNIT_TEST
    digitalWrite(_buzzerPin, HIGH);
    _buzzerEndTime = millis() + 40;
    _buzzerActive = true;
#endif
}

void IndicatorController::playAlertBeep() {
#ifndef NATIVE_UNIT_TEST
    digitalWrite(_buzzerPin, HIGH);
    _buzzerEndTime = millis() + 100;
    _buzzerActive = true;
#endif
}

void IndicatorController::update(uint32_t nowMs) {
#ifndef NATIVE_UNIT_TEST
    // 1. Manage active buzzer cut-off
    if (_buzzerActive && nowMs >= _buzzerEndTime) {
        digitalWrite(_buzzerPin, LOW);
        _buzzerActive = false;
    }

    // 2. Periodic low battery chirp
    if (_status.test(STATUS_BIT_LOW_BATTERY) && _status.test(STATUS_BIT_POWER_ON)) {
        if ((nowMs - _lastLowBatteryChirp) >= LOW_BATTERY_ALERT_MS) {
            _lastLowBatteryChirp = nowMs;
            playAlertBeep();
        }
    }

    // 3. LED Cadence Generator
    if (!_status.test(STATUS_BIT_POWER_ON)) {
        // System OFF -> LED OFF
        digitalWrite(_ledPin, LOW);
        return;
    }

    if (_status.test(STATUS_BIT_FAULT_WATCHDOG) || _status.test(STATUS_BIT_LOW_BATTERY)) {
        // Fault / Low battery: Fast flash (150 ms period)
        if ((nowMs - _lastLedToggle) >= 150) {
            _lastLedToggle = nowMs;
            _ledState = !_ledState;
            digitalWrite(_ledPin, _ledState ? HIGH : LOW);
        }
    } else if (_status.test(STATUS_BIT_MANUAL_MODE)) {
        // Manual mode: Gentle blink (500 ms period)
        if ((nowMs - _lastLedToggle) >= 500) {
            _lastLedToggle = nowMs;
            _ledState = !_ledState;
            digitalWrite(_ledPin, _ledState ? HIGH : LOW);
        }
    } else {
        // Normal Auto mode: Solid ON
        digitalWrite(_ledPin, HIGH);
    }
#endif
}
