#include "battery.h"
#include "../config.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#endif

// 2S Li-ion discharge curve lookup points: {Voltage (V), Percentage (%)}
struct VoltagePctPoint {
    float voltage;
    uint8_t percentage;
};

static const VoltagePctPoint DISCHARGE_CURVE[] = {
    { 6.00f,   0 },
    { 6.60f,   5 },
    { 6.80f,  10 },
    { 7.00f,  20 },
    { 7.25f,  35 },
    { 7.40f,  50 },
    { 7.60f,  65 },
    { 7.85f,  80 },
    { 8.10f,  90 },
    { 8.30f,  97 },
    { 8.40f, 100 }
};

static const size_t CURVE_POINTS_COUNT = sizeof(DISCHARGE_CURVE) / sizeof(DISCHARGE_CURVE[0]);

BatteryMonitor::BatteryMonitor()
    : _adcPin(PIN_BATTERY_ADC),
      _filteredAdc(0.0f),
      _initialized(false)
{
    _status = { 7.4f, 50, false, false };
}

void BatteryMonitor::begin(int adcPin) {
    _adcPin = adcPin;
#ifndef NATIVE_UNIT_TEST
    pinMode(_adcPin, INPUT);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db); // Up to ~2.6V - 3.1V full scale
#endif
    _initialized = true;
}

uint8_t BatteryMonitor::voltageToPercentage(float voltage) {
    if (voltage <= DISCHARGE_CURVE[0].voltage) {
        return 0;
    }
    if (voltage >= DISCHARGE_CURVE[CURVE_POINTS_COUNT - 1].voltage) {
        return 100;
    }

    for (size_t i = 0; i < CURVE_POINTS_COUNT - 1; i++) {
        if (voltage >= DISCHARGE_CURVE[i].voltage && voltage <= DISCHARGE_CURVE[i + 1].voltage) {
            float vRange = DISCHARGE_CURVE[i + 1].voltage - DISCHARGE_CURVE[i].voltage;
            float vFraction = (voltage - DISCHARGE_CURVE[i].voltage) / vRange;
            float pctRange = (float)(DISCHARGE_CURVE[i + 1].percentage - DISCHARGE_CURVE[i].percentage);
            return (uint8_t)(DISCHARGE_CURVE[i].percentage + (vFraction * pctRange) + 0.5f);
        }
    }

    return 50;
}

BatteryStatus BatteryMonitor::update() {
#ifndef NATIVE_UNIT_TEST
    if (!_initialized) {
        begin(_adcPin);
    }

    // Oversample ADC (8 samples) to reduce noise
    uint32_t adcSum = 0;
    for (int i = 0; i < 8; i++) {
        adcSum += analogRead(_adcPin);
        delayMicroseconds(50);
    }
    float rawAdc = (float)adcSum / 8.0f;

    // First sample initialization vs EMA filter (alpha = 0.2)
    if (_filteredAdc <= 0.0f) {
        _filteredAdc = rawAdc;
    } else {
        _filteredAdc = 0.2f * rawAdc + 0.8f * _filteredAdc;
    }

    // Convert ADC to pin voltage
    float pinVoltage = (_filteredAdc / BATTERY_ADC_RESOLUTION) * BATTERY_ADC_VREF;

    // Reconstruct pack voltage across voltage divider: V_pack = V_pin * (R1 + R2) / R2
    float packVoltage = pinVoltage * ((BATTERY_R1_OHMS + BATTERY_R2_OHMS) / BATTERY_R2_OHMS);
#else
    float packVoltage = 7.4f;
#endif

    _status.voltage = packVoltage;
    _status.percentage = voltageToPercentage(packVoltage);
    _status.isLow = (_status.percentage < 10);
    _status.isCritical = (_status.voltage < 6.0f);

    return _status;
}
