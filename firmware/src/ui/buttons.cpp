#include "buttons.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#endif

DebouncedButton::DebouncedButton(int pin, uint32_t debounceMs, uint32_t longPressMs)
    : _pin(pin),
      _debounceMs(debounceMs),
      _longPressMs(longPressMs),
      _lastPhysicalState(true),
      _debouncedState(true),
      _lastChangeTime(0),
      _pressStartTime(0),
      _longPressTriggered(false)
{
}

void DebouncedButton::begin() {
#ifndef NATIVE_UNIT_TEST
    pinMode(_pin, INPUT_PULLUP);
    _lastPhysicalState = digitalRead(_pin);
    _debouncedState = _lastPhysicalState;
#endif
}

ButtonEvent DebouncedButton::update(uint32_t nowMs) {
#ifndef NATIVE_UNIT_TEST
    bool physicalState = digitalRead(_pin);
#else
    bool physicalState = true;
#endif

    ButtonEvent event = BTN_EVENT_NONE;

    // Check for raw physical edge
    if (physicalState != _lastPhysicalState) {
        _lastPhysicalState = physicalState;
        _lastChangeTime = nowMs;
    }

    // Debounce filter
    if ((nowMs - _lastChangeTime) >= _debounceMs) {
        // State transition detected
        if (physicalState != _debouncedState) {
            _debouncedState = physicalState;

            // Pressed (Active LOW)
            if (_debouncedState == false) {
                _pressStartTime = nowMs;
                _longPressTriggered = false;
            } else {
                // Released
                if (!_longPressTriggered) {
                    event = BTN_EVENT_SHORT_PRESS;
                }
            }
        }
    }

    // Check long press while held down
    if (_debouncedState == false && !_longPressTriggered) {
        if ((nowMs - _pressStartTime) >= _longPressMs) {
            _longPressTriggered = true;
            event = BTN_EVENT_LONG_PRESS;
        }
    }

    return event;
}
