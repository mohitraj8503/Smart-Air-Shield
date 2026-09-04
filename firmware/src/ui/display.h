#ifndef SMART_AIR_SHIELD_DISPLAY_H
#define SMART_AIR_SHIELD_DISPLAY_H

#include <stdint.h>
#include "../control/aqi.h"
#include "../control/blower_control.h"

struct DisplayData {
    int aqiValue;
    int aqiBucket;
    uint16_t pm2_5_ambient;
    uint16_t pm10_ambient;
    uint16_t pm2_5_outlet;
    uint16_t pm10_outlet;
    OperatingMode mode;
    uint8_t fanDutyPct;
    uint8_t batteryPct;
    bool isWarmingUp;
    uint8_t warmupRemainingSec;
    bool sensorFault;
    bool isBleConnected;
    bool isPoweredOn;
};

class DisplayManager {
public:
    DisplayManager();

    bool begin();
    void render(const DisplayData& data);
    void showSplashScreen();
    void showPowerOffScreen();

private:
    void drawHeader(const DisplayData& data);
    void drawSensorData(const DisplayData& data);
    void drawStatusBar(const DisplayData& data);
    void drawWarmupScreen(uint8_t remainingSec);

    bool _initialized;
};

#endif // SMART_AIR_SHIELD_DISPLAY_H
