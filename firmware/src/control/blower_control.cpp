/**
 * Adapted from Harnas/ESP32_Airpurifier2 (lib/Fan/Fan.cpp & src/AirCleanerController.cpp)
 * Enhanced for dual-sensor comparative monitoring with discrete AQI-bucket table & slew-rate limiting.
 */

#include "blower_control.h"
#include "../config.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#include <esp32-hal-ledc.h>
#endif

BlowerController::BlowerController()
    : _mode(MODE_AUTO),
      _lastActiveMode(MODE_AUTO),
      _manualDuty(MANUAL_DUTY_MED),
      _currentPreset(MANUAL_PRESET_MED),
      _targetDuty(AQI_BUCKET_TO_DUTY_PCT[0]),
      _currentDuty(0),
      _pwmPin(PIN_BLOWER_PWM),
      _pwmChannel(BLOWER_PWM_CHANNEL),
      _pwmMaxTicks(BLOWER_PWM_MAX_TICKS),
      _initialized(false)
{
}

void BlowerController::begin(int pwmPin, int pwmChannel, int freqHz, int resolutionBits) {
    _pwmPin = pwmPin;
    _pwmChannel = pwmChannel;
    _pwmMaxTicks = (1 << resolutionBits) - 1;

#ifndef NATIVE_UNIT_TEST
    ledcSetup(_pwmChannel, freqHz, resolutionBits);
    ledcAttachPin(_pwmPin, _pwmChannel);
    ledcWrite(_pwmChannel, 0);
#endif
    _initialized = true;
}

uint8_t BlowerController::computeSlewStep(uint8_t current, uint8_t target, uint8_t maxStep) {
    if (current == target) {
        return target;
    }

    if (target > current) {
        uint16_t next = (uint16_t)current + maxStep;
        return (next >= target) ? target : (uint8_t)next;
    } else {
        if (current <= maxStep || (current - maxStep) <= target) {
            return target;
        }
        return current - maxStep;
    }
}

void BlowerController::applyPwm(uint8_t dutyPct) {
#ifndef NATIVE_UNIT_TEST
    if (!_initialized) return;

    if (dutyPct == 0) {
        ledcWrite(_pwmChannel, 0);
    } else {
        // Map 0 - 100% to 0 - _pwmMaxTicks
        uint32_t ticks = ((uint32_t)dutyPct * _pwmMaxTicks) / 100;
        ledcWrite(_pwmChannel, ticks);
    }
#endif
}

uint8_t BlowerController::update(int ambientAqiBucket, bool sensorFault) {
    // 1. Determine raw target duty based on mode
    if (_mode == MODE_OFF) {
        _targetDuty = 0;
    } else if (sensorFault) {
        // Fallback to safe medium duty if sensor timed out
        _targetDuty = BLOWER_FALLBACK_DUTY;
    } else if (_mode == MODE_MANUAL) {
        _targetDuty = _manualDuty;
    } else {
        // Auto Mode: Map bucket to discrete duty lookup table
        if (ambientAqiBucket < 0) ambientAqiBucket = 0;
        if (ambientAqiBucket > 5) ambientAqiBucket = 5;

        uint8_t mappedDuty = AQI_BUCKET_TO_DUTY_PCT[ambientAqiBucket];

        // Clamp to Auto mode acoustic ceiling (< 35 dB @ 1m)
        if (mappedDuty > BLOWER_MAX_AUTO_DUTY) {
            mappedDuty = BLOWER_MAX_AUTO_DUTY;
        }
        // Clamp to min active duty to prevent stall
        if (mappedDuty < BLOWER_MIN_ACTIVE_DUTY) {
            mappedDuty = BLOWER_MIN_ACTIVE_DUTY;
        }

        _targetDuty = mappedDuty;
    }

    // 2. Apply slew-rate limiter
    _currentDuty = computeSlewStep(_currentDuty, _targetDuty, BLOWER_SLEW_STEP_PCT);

    // 3. Write duty to hardware PWM
    applyPwm(_currentDuty);

    return _currentDuty;
}

void BlowerController::setMode(OperatingMode mode) {
    if (_mode != mode) {
        if (mode != MODE_OFF) {
            _lastActiveMode = mode;
        }
        _mode = mode;
    }
}

void BlowerController::setManualDuty(uint8_t dutyPct) {
    if (dutyPct > BLOWER_MAX_MANUAL_DUTY) {
        dutyPct = BLOWER_MAX_MANUAL_DUTY;
    }
    if (dutyPct < BLOWER_MIN_ACTIVE_DUTY && dutyPct != 0) {
        dutyPct = BLOWER_MIN_ACTIVE_DUTY;
    }
    _manualDuty = dutyPct;
    _currentPreset = MANUAL_PRESET_CUSTOM;
    setMode(MODE_MANUAL);
}

void BlowerController::cycleManualPreset() {
    if (_mode != MODE_MANUAL) {
        setMode(MODE_MANUAL);
        _currentPreset = MANUAL_PRESET_LOW;
        _manualDuty = MANUAL_DUTY_LOW;
        return;
    }

    switch (_currentPreset) {
        case MANUAL_PRESET_LOW:
            _currentPreset = MANUAL_PRESET_MED;
            _manualDuty = MANUAL_DUTY_MED;
            break;
        case MANUAL_PRESET_MED:
            _currentPreset = MANUAL_PRESET_HIGH;
            _manualDuty = MANUAL_DUTY_HIGH;
            break;
        case MANUAL_PRESET_HIGH:
        case MANUAL_PRESET_CUSTOM:
        default:
            // Cycle back to Auto mode
            setMode(MODE_AUTO);
            break;
    }
}

void BlowerController::togglePower() {
    if (_mode == MODE_OFF) {
        // Turn ON to last active mode
        setMode(_lastActiveMode == MODE_OFF ? MODE_AUTO : _lastActiveMode);
    } else {
        // Turn OFF
        setMode(MODE_OFF);
    }
}
