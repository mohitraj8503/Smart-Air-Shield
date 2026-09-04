#ifndef SMART_AIR_SHIELD_BATTERY_H
#define SMART_AIR_SHIELD_BATTERY_H

#include <stdint.h>

struct BatteryStatus {
    float voltage;       // Battery pack voltage in Volts (e.g. 7.4V)
    uint8_t percentage;  // 0 - 100%
    bool isLow;          // True if percentage < 10%
    bool isCritical;     // True if voltage < 6.0V
};

class BatteryMonitor {
public:
    BatteryMonitor();

    void begin(int adcPin);

    // Reads ADC, updates filtered voltage and percentage
    BatteryStatus update();

    BatteryStatus getStatus() const { return _status; }

    // Static conversion from voltage (V) to percentage using non-linear 2S curve
    static uint8_t voltageToPercentage(float voltage);

private:
    int _adcPin;
    float _filteredAdc;
    BatteryStatus _status;
    bool _initialized;
};

#endif // SMART_AIR_SHIELD_BATTERY_H
