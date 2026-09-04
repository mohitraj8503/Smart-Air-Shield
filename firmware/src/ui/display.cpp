#include "display.h"
#include "../config.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Full buffer hardware I2C SSD1306 display instance
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ PIN_OLED_SCL, /* data=*/ PIN_OLED_SDA);
#endif

DisplayManager::DisplayManager()
    : _initialized(false)
{
}

bool DisplayManager::begin() {
#ifndef NATIVE_UNIT_TEST
    Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
    if (!u8g2.begin()) {
        return false;
    }
    u8g2.clearBuffer();
    showSplashScreen();
#endif
    _initialized = true;
    return true;
}

void DisplayManager::showSplashScreen() {
#ifndef NATIVE_UNIT_TEST
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(10, 20, "SMART AIR-SHIELD");

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(12, 38, "Helmet Purification");
    u8g2.drawStr(22, 52, "Vishwakarma 26-27");
    u8g2.sendBuffer();
#endif
}

void DisplayManager::showPowerOffScreen() {
#ifndef NATIVE_UNIT_TEST
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14B_tf);
    u8g2.drawStr(28, 30, "STANDBY");
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(18, 48, "Press Power to Wake");
    u8g2.sendBuffer();
#endif
}

void DisplayManager::drawWarmupScreen(uint8_t remainingSec) {
#ifndef NATIVE_UNIT_TEST
    u8g2.setFont(u8g2_font_7x14B_tf);
    u8g2.drawStr(6, 16, "SMART AIR-SHIELD");

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(12, 32, "Laser Diode Warmup");

    char buf[24];
    snprintf(buf, sizeof(buf), "Stabilizing: %d s", remainingSec);
    u8g2.drawStr(16, 46, buf);

    // Progress bar (0 to 100 px wide)
    int total = SENSOR_WARMUP_TIME_MS / 1000;
    int elapsed = total - remainingSec;
    if (elapsed < 0) elapsed = 0;
    int progress = (elapsed * 100) / total;

    u8g2.drawFrame(14, 52, 100, 7);
    u8g2.drawBox(14, 52, progress, 7);
#endif
}

void DisplayManager::drawHeader(const DisplayData& data) {
#ifndef NATIVE_UNIT_TEST
    char aqiStr[24];
    const char* cat = aqi_bucket_to_category_str(data.aqiBucket);

    snprintf(aqiStr, sizeof(aqiStr), "AQI %d  %s", data.aqiValue, cat);

    // High AQI: inverted highlight box
    if (data.aqiBucket >= AQI_BUCKET_UNHEALTHY) {
        u8g2.drawBox(0, 0, 128, 14);
        u8g2.setDrawColor(0); // White text on black box
        u8g2.setFont(u8g2_font_7x14B_tf);
        u8g2.drawStr(2, 12, aqiStr);
        u8g2.setDrawColor(1); // Reset
    } else {
        u8g2.setFont(u8g2_font_7x14B_tf);
        u8g2.drawStr(2, 12, aqiStr);
        u8g2.drawHLine(0, 14, 128);
    }
#endif
}

void DisplayManager::drawSensorData(const DisplayData& data) {
#ifndef NATIVE_UNIT_TEST
    char pm25Str[32];
    char pm10Str[32];

    u8g2.setFont(u8g2_font_6x10_tf);

    // Show ambient PM with delivered outlet comparison
    // Format: "PM2.5: 128 -> 14 ug/m3" or if outlet unavailable "PM2.5: 128 ug/m3"
    if (data.pm2_5_outlet > 0 && data.pm2_5_outlet < data.pm2_5_ambient) {
        snprintf(pm25Str, sizeof(pm25Str), "PM2.5: %3d -> %2d", data.pm2_5_ambient, data.pm2_5_outlet);
    } else {
        snprintf(pm25Str, sizeof(pm25Str), "PM2.5: %3d ug/m3", data.pm2_5_ambient);
    }

    snprintf(pm10Str, sizeof(pm10Str), "PM10 : %3d ug/m3", data.pm10_ambient);

    u8g2.drawStr(2, 28, pm25Str);
    u8g2.drawStr(2, 42, pm10Str);
#endif
}

void DisplayManager::drawStatusBar(const DisplayData& data) {
#ifndef NATIVE_UNIT_TEST
    u8g2.drawHLine(0, 48, 128);
    u8g2.setFont(u8g2_font_6x10_tf);

    // Left: Mode & Blower Speed
    char modeStr[16];
    if (data.mode == MODE_OFF) {
        snprintf(modeStr, sizeof(modeStr), "FAN OFF");
    } else if (data.mode == MODE_AUTO) {
        snprintf(modeStr, sizeof(modeStr), "AUTO %d%%", data.fanDutyPct);
    } else {
        snprintf(modeStr, sizeof(modeStr), "MAN  %d%%", data.fanDutyPct);
    }
    u8g2.drawStr(2, 60, modeStr);

    // Center icon indicators (BLE paired / Sensor fault)
    if (data.sensorFault) {
        u8g2.drawStr(66, 60, "!ERR");
    } else if (data.isBleConnected) {
        u8g2.drawStr(70, 60, "BLE");
    }

    // Right: Battery Icon & Percentage
    char batStr[12];
    snprintf(batStr, sizeof(batStr), "%d%%", data.batteryPct);
    int batX = (data.batteryPct >= 100) ? 102 : 108;
    u8g2.drawStr(batX, 60, batStr);

    // Battery outline glyph
    u8g2.drawFrame(93, 52, 12, 9);
    u8g2.drawBox(105, 55, 2, 3); // Battery nipple

    // Fill battery indicator proportional to charge
    int fillW = (data.batteryPct * 8) / 100;
    if (fillW > 8) fillW = 8;
    if (fillW > 0) {
        u8g2.drawBox(95, 54, fillW, 5);
    }
#endif
}

void DisplayManager::render(const DisplayData& data) {
#ifndef NATIVE_UNIT_TEST
    if (!_initialized) return;

    u8g2.clearBuffer();

    if (!data.isPoweredOn) {
        showPowerOffScreen();
        return;
    }

    if (data.isWarmingUp) {
        drawWarmupScreen(data.warmupRemainingSec);
    } else {
        drawHeader(data);
        drawSensorData(data);
        drawStatusBar(data);
    }

    u8g2.sendBuffer();
#endif
}
