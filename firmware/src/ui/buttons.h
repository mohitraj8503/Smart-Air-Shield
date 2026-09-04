#ifndef SMART_AIR_SHIELD_BUTTONS_H
#define SMART_AIR_SHIELD_BUTTONS_H

#include <stdint.h>

enum ButtonEvent {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_SHORT_PRESS,
    BTN_EVENT_LONG_PRESS
};

class DebouncedButton {
public:
    DebouncedButton(int pin, uint32_t debounceMs = 50, uint32_t longPressMs = 1200);

    void begin();
    ButtonEvent update(uint32_t nowMs);

private:
    int _pin;
    uint32_t _debounceMs;
    uint32_t _longPressMs;
    bool _lastPhysicalState;
    bool _debouncedState;
    uint32_t _lastChangeTime;
    uint32_t _pressStartTime;
    bool _longPressTriggered;
};

#endif // SMART_AIR_SHIELD_BUTTONS_H
