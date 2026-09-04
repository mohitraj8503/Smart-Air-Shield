/**
 * SMART AIR-SHIELD — Main Firmware Entrypoint
 * 
 * Removable Air-Purification & Monitoring Module for Two-Wheeler Helmets
 * Vishwakarma Awards 2026-27 (Theme: Sustainable Cities)
 * 
 * FreeRTOS task architecture:
 * - vSensorTask: PMS7003 dual UART reads + battery ADC
 * - vControlTask: Adaptive blower control law + slew rate limiter (200ms)
 * - vUITask: Debounced buttons, U8g2 OLED rendering (2Hz), LED/Buzzer
 * - vTelemetryTask: BLE notify (1Hz), LittleFS session logging, HTTP export
 */

#ifndef NATIVE_UNIT_TEST

#include <Arduino.h>
#include <HardwareSerial.h>
#include "config.h"
#include "sensors/pms_sensor.h"
#include "sensors/battery.h"
#include "control/aqi.h"
#include "control/blower_control.h"
#include "ui/display.h"
#include "ui/buttons.h"
#include "ui/led_buzzer.h"
#include "connectivity/ble_service.h"
#include "connectivity/wifi_logger.h"
#include "storage/session_log.h"

// Hardware Serial Instances for Dual PMS7003 Sensors
static HardwareSerial SerialPM_Amb(1);
static HardwareSerial SerialPM_Out(2);

// Subsystem Instances
static PMS7003Sensor s_sensorAmb;
static PMS7003Sensor s_sensorOut;
static BatteryMonitor s_battery;
static BlowerController s_blower;
static DisplayManager s_display;
static DebouncedButton s_btnMode(PIN_BTN_MODE, BTN_DEBOUNCE_MS, BTN_LONG_PRESS_MS);
static DebouncedButton s_btnPower(PIN_BTN_POWER, BTN_DEBOUNCE_MS, BTN_LONG_PRESS_MS);
static IndicatorController s_indicators;
static BleServiceManager s_ble;
static WifiLogger s_wifiLogger;
static SessionLogger s_sessionLog;

// Global Thread-Safe State Cache
static portMUX_TYPE s_stateMux = portMUX_INITIALIZER_UNLOCKED;

struct SharedState {
    PMReading ambReading;
    PMReading outReading;
    BatteryStatus battery;
    int aqiValue;
    int aqiBucket;
    uint8_t targetDuty;
    uint8_t currentDuty;
    OperatingMode mode;
    bool ambFault;
    bool outFault;
    bool isWarmingUp;
    uint8_t warmupRemainingSec;
    bool powerOn;
} s_sharedState;

// BLE Control Callback
void handleBleCommand(BleCommandType cmd, uint8_t value) {
    portENTER_CRITICAL(&s_stateMux);
    switch (cmd) {
        case CMD_SET_MODE:
            if (value == 0) {
                s_blower.setMode(MODE_OFF);
                s_sharedState.powerOn = false;
            } else if (value == 1) {
                s_blower.setMode(MODE_AUTO);
                s_sharedState.powerOn = true;
            } else if (value == 2) {
                s_blower.setMode(MODE_MANUAL);
                s_sharedState.powerOn = true;
            }
            break;
        case CMD_SET_DUTY:
            s_blower.setManualDuty(value);
            s_sharedState.powerOn = true;
            break;
        case CMD_POWER_TOGGLE:
            s_blower.togglePower();
            s_sharedState.powerOn = s_blower.isPoweredOn();
            break;
    }
    portEXIT_CRITICAL(&s_stateMux);
    s_indicators.playModeChangeBeep();
}

// ----------------------------------------------------------------------------
// FreeRTOS Task: Sensor Polling & Battery ADC
// ----------------------------------------------------------------------------
void vSensorTask(void* pvParameters) {
    uint32_t lastBatteryCheck = 0;

    for (;;) {
        uint32_t now = millis();

        // 1. Update PMS7003 Sensors
        s_sensorAmb.update(now);
        s_sensorOut.update(now);

        // 2. Battery Monitoring (Every 5s)
        if ((now - lastBatteryCheck) >= BATTERY_READ_INTERVAL_MS) {
            lastBatteryCheck = now;
            BatteryStatus bat = s_battery.update();

            portENTER_CRITICAL(&s_stateMux);
            s_sharedState.battery = bat;
            portEXIT_CRITICAL(&s_stateMux);

            s_indicators.setBit(STATUS_BIT_LOW_BATTERY, bat.isLow);
        }

        // 3. Sensor Fault / Watchdog Check (>5s timeout)
        bool ambTimeout = s_sensorAmb.isTimedOut(now, SENSOR_WATCHDOG_TIMEOUT);
        bool outTimeout = s_sensorOut.isTimedOut(now, SENSOR_WATCHDOG_TIMEOUT);

        s_indicators.setBit(STATUS_BIT_AMB_SENSOR_OK, !ambTimeout);
        s_indicators.setBit(STATUS_BIT_OUT_SENSOR_OK, !outTimeout);
        s_indicators.setBit(STATUS_BIT_FAULT_WATCHDOG, ambTimeout || outTimeout);

        // 4. Update Warmup & Shared Readings
        bool warmingUp = !s_sensorAmb.isWarmedUp(now);
        uint8_t remainSec = 0;
        if (warmingUp) {
            remainSec = (uint8_t)((SENSOR_WARMUP_TIME_MS - now) / 1000);
        }

        PMReading amb = s_sensorAmb.getLatestReading();
        PMReading out = s_sensorOut.getLatestReading();

        portENTER_CRITICAL(&s_stateMux);
        s_sharedState.ambReading = amb;
        s_sharedState.outReading = out;
        s_sharedState.ambFault = ambTimeout;
        s_sharedState.outFault = outTimeout;
        s_sharedState.isWarmingUp = warmingUp;
        s_sharedState.warmupRemainingSec = remainSec;
        portEXIT_CRITICAL(&s_stateMux);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// ----------------------------------------------------------------------------
// FreeRTOS Task: Blower Control Law & Slew-Rate Limiter (200ms tick)
// ----------------------------------------------------------------------------
void vControlTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Read current inputs
        portENTER_CRITICAL(&s_stateMux);
        float pm25 = (float)s_sharedState.ambReading.pm2_5;
        bool fault = s_sharedState.ambFault;
        portEXIT_CRITICAL(&s_stateMux);

        // Calculate AQI and bucket
        int bucket = pm25_to_aqi_bucket(pm25);
        int aqi = pm25_to_aqi_value(pm25);

        // Run adaptive blower control update
        uint8_t duty = s_blower.update(bucket, fault);

        portENTER_CRITICAL(&s_stateMux);
        s_sharedState.aqiValue = aqi;
        s_sharedState.aqiBucket = bucket;
        s_sharedState.currentDuty = duty;
        s_sharedState.targetDuty = s_blower.getTargetDuty();
        s_sharedState.mode = s_blower.getMode();
        s_sharedState.powerOn = s_blower.isPoweredOn();
        portEXIT_CRITICAL(&s_stateMux);

        // Sleep until next 200 ms tick
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(BLOWER_SLEW_INTERVAL_MS));
    }
}

// ----------------------------------------------------------------------------
// FreeRTOS Task: UI (Buttons, OLED Display at 2Hz, LED, Buzzer)
// ----------------------------------------------------------------------------
void vUITask(void* pvParameters) {
    uint32_t lastOledRefresh = 0;

    for (;;) {
        uint32_t now = millis();

        // 1. Button Updates
        ButtonEvent modeEvt = s_btnMode.update(now);
        ButtonEvent pwrEvt = s_btnPower.update(now);

        if (modeEvt == BTN_EVENT_SHORT_PRESS) {
            s_blower.cycleManualPreset();
            s_indicators.playModeChangeBeep();
        }

        if (pwrEvt == BTN_EVENT_SHORT_PRESS) {
            s_blower.togglePower();
            if (s_blower.isPoweredOn()) {
                s_indicators.playPowerOnTone();
            } else {
                s_indicators.playPowerOffTone();
            }
        } else if (pwrEvt == BTN_EVENT_LONG_PRESS) {
            // Toggle WiFi Access Point for log download
            if (s_wifiLogger.isRunning()) {
                s_wifiLogger.stop();
                s_indicators.setBit(STATUS_BIT_WIFI_ACTIVE, false);
            } else {
                s_wifiLogger.startAccessPoint();
                s_indicators.setBit(STATUS_BIT_WIFI_ACTIVE, true);
            }
            s_indicators.playAlertBeep();
        }

        // 2. Update Indicator Status Bits
        s_indicators.setBit(STATUS_BIT_MANUAL_MODE, s_blower.getMode() == MODE_MANUAL);
        s_indicators.setBit(STATUS_BIT_POWER_ON, s_blower.isPoweredOn());
        s_indicators.setBit(STATUS_BIT_BLE_CONNECTED, s_ble.isConnected());
        s_indicators.update(now);

        // 3. Render OLED Display at 2 Hz
        if ((now - lastOledRefresh) >= OLED_REFRESH_INTERVAL_MS) {
            lastOledRefresh = now;

            DisplayData disp;
            portENTER_CRITICAL(&s_stateMux);
            disp.aqiValue = s_sharedState.aqiValue;
            disp.aqiBucket = s_sharedState.aqiBucket;
            disp.pm2_5_ambient = s_sharedState.ambReading.pm2_5;
            disp.pm10_ambient = s_sharedState.ambReading.pm10;
            disp.pm2_5_outlet = s_sharedState.outReading.pm2_5;
            disp.pm10_outlet = s_sharedState.outReading.pm10;
            disp.mode = s_sharedState.mode;
            disp.fanDutyPct = s_sharedState.currentDuty;
            disp.batteryPct = s_sharedState.battery.percentage;
            disp.isWarmingUp = s_sharedState.isWarmingUp;
            disp.warmupRemainingSec = s_sharedState.warmupRemainingSec;
            disp.sensorFault = s_sharedState.ambFault || s_sharedState.outFault;
            disp.isBleConnected = s_ble.isConnected();
            disp.isPoweredOn = s_sharedState.powerOn;
            portEXIT_CRITICAL(&s_stateMux);

            s_display.render(disp);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// ----------------------------------------------------------------------------
// FreeRTOS Task: Telemetry & Logging (BLE 1Hz, LittleFS, WiFi export)
// ----------------------------------------------------------------------------
void vTelemetryTask(void* pvParameters) {
    uint32_t lastLogFlush = 0;

    for (;;) {
        uint32_t now = millis();

        // Build 17-byte packed telemetry packet
        TelemetryPacket pkt;
        portENTER_CRITICAL(&s_stateMux);
        pkt.timestamp = now;
        pkt.pm2_5_ambient = s_sharedState.ambReading.pm2_5;
        pkt.pm10_ambient = s_sharedState.ambReading.pm10;
        pkt.pm2_5_outlet = s_sharedState.outReading.pm2_5;
        pkt.pm10_outlet = s_sharedState.outReading.pm10;
        pkt.aqi_value = (uint16_t)s_sharedState.aqiValue;
        pkt.aqi_bucket = (uint8_t)s_sharedState.aqiBucket;
        pkt.blower_duty = s_sharedState.currentDuty;
        pkt.battery_pct = s_sharedState.battery.percentage;
        pkt.operating_mode = (uint8_t)s_sharedState.mode;
        pkt.status_flags = s_indicators.getStatusByte();
        portEXIT_CRITICAL(&s_stateMux);

        // 1. Notify connected BLE companion app
        s_ble.updateTelemetry(pkt);

        // 2. Buffer for session log
        SessionRecord rec;
        rec.timestamp = pkt.timestamp;
        rec.pm2_5_ambient = pkt.pm2_5_ambient;
        rec.pm10_ambient = pkt.pm10_ambient;
        rec.pm2_5_outlet = pkt.pm2_5_outlet;
        rec.pm10_outlet = pkt.pm10_outlet;
        rec.aqi = pkt.aqi_value;
        rec.duty = pkt.blower_duty;
        rec.battery = pkt.battery_pct;
        rec.status = pkt.status_flags;
        s_sessionLog.logSample(rec);

        // 3. Periodic flush to LittleFS CSV
        if ((now - lastLogFlush) >= LOG_FLUSH_INTERVAL_MS) {
            lastLogFlush = now;
            s_sessionLog.flush();
        }

        // 4. Update and handle WiFi if enabled
        if (s_wifiLogger.isRunning()) {
            s_wifiLogger.updateTelemetry(pkt);
            s_wifiLogger.handleClient();
        }

        vTaskDelay(pdMS_TO_TICKS(BLE_NOTIFY_INTERVAL_MS));
    }
}

// ----------------------------------------------------------------------------
// Arduino Setup & Main Loop
// ----------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println("\n[INIT] SMART AIR-SHIELD initializing...");

    // Initialize Shared State
    s_sharedState.ambReading = {0, 0, 0, 0, 0, 0, 0, false};
    s_sharedState.outReading = {0, 0, 0, 0, 0, 0, 0, false};
    s_sharedState.battery = {7.4f, 50, false, false};
    s_sharedState.aqiValue = 0;
    s_sharedState.aqiBucket = 0;
    s_sharedState.targetDuty = 20;
    s_sharedState.currentDuty = 0;
    s_sharedState.mode = MODE_AUTO;
    s_sharedState.ambFault = false;
    s_sharedState.outFault = false;
    s_sharedState.isWarmingUp = true;
    s_sharedState.warmupRemainingSec = SENSOR_WARMUP_TIME_MS / 1000;
    s_sharedState.powerOn = true;

    // Initialize Subsystems
    s_sessionLog.begin();
    s_indicators.begin(PIN_STATUS_LED, PIN_BUZZER);
    s_display.begin();
    s_btnMode.begin();
    s_btnPower.begin();
    s_blower.begin(PIN_BLOWER_PWM, BLOWER_PWM_CHANNEL, BLOWER_PWM_FREQ_HZ, BLOWER_PWM_RES_BITS);
    s_battery.begin(PIN_BATTERY_ADC);

    // Initialize Dual PMS7003 Sensors
    s_sensorAmb.begin(&SerialPM_Amb, PIN_PM_AMB_RX, PIN_PM_AMB_TX);
    s_sensorOut.begin(&SerialPM_Out, PIN_PM_OUT_RX, PIN_PM_OUT_TX);

    // Initialize NimBLE GATT Server
    s_ble.begin(handleBleCommand);

    s_indicators.playPowerOnTone();
    Serial.println("[INIT] SMART AIR-SHIELD subsystems ready.");

    // Launch FreeRTOS Tasks
    xTaskCreatePinnedToCore(vSensorTask,    "SensorTask",    TASK_SENSOR_STACK_SIZE,  NULL, TASK_SENSOR_PRIORITY,  NULL, 1);
    xTaskCreatePinnedToCore(vControlTask,   "ControlTask",   TASK_CONTROL_STACK_SIZE, NULL, TASK_CONTROL_PRIORITY, NULL, 1);
    xTaskCreatePinnedToCore(vUITask,        "UITask",        TASK_UI_STACK_SIZE,       NULL, TASK_UI_PRIORITY,      NULL, 0);
    xTaskCreatePinnedToCore(vTelemetryTask, "TelemetryTask", TASK_BLE_STACK_SIZE,      NULL, TASK_BLE_PRIORITY,     NULL, 0);
}

void loop() {
    // FreeRTOS tasks handle all work; yield CPU in loop
    vTaskDelay(pdMS_TO_TICKS(1000));
}

#else
// Minimal stub when running under native unit test compilation
int main() {
    return 0;
}
#endif
