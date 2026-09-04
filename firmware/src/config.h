#ifndef SMART_AIR_SHIELD_CONFIG_H
#define SMART_AIR_SHIELD_CONFIG_H

#include <stdint.h>

/**
 * SMART AIR-SHIELD Configuration & Pin Mapping
 * 
 * Centralized configuration repository for all GPIO pins, hardware tunables,
 * control laws, timing constants, and BLE UUIDs.
 * 
 * Submission for Vishwakarma Awards 2026-27 (Theme: Sustainable Cities)
 */

// ============================================================================
// GPIO PIN ASSIGNMENTS (ESP32-WROOM-32)
// ============================================================================

// Ambient PM Sensor (PMS7003 on HardwareSerial 1)
#define PIN_PM_AMB_RX           16
#define PIN_PM_AMB_TX           17

// Outlet / Breathing-Zone PM Sensor (PMS7003 on HardwareSerial 2)
#define PIN_PM_OUT_RX           25
#define PIN_PM_OUT_TX           26

// I2C OLED Display (SSD1306 128x64)
#define PIN_OLED_SDA            21
#define PIN_OLED_SCL            22
#define OLED_I2C_ADDR           0x3C

// Blower PWM Driver (Centrifugal Brushless Blower via MOSFET)
#define PIN_BLOWER_PWM          27
#define BLOWER_PWM_CHANNEL      0
#define BLOWER_PWM_FREQ_HZ      25000   // 25 kHz: ultrasonic / above human audible range
#define BLOWER_PWM_RES_BITS     10      // 10-bit: 0 - 1023 duty cycle
#define BLOWER_PWM_MAX_TICKS    1023

// User Control Tactile Buttons (Active LOW with internal pull-up)
#define PIN_BTN_MODE            32
#define PIN_BTN_POWER           33
#define BTN_DEBOUNCE_MS         50
#define BTN_LONG_PRESS_MS       1200

// Status Indicators
#define PIN_STATUS_LED          2       // GPIO2 status LED
#define PIN_BUZZER              15      // Passive/Active buzzer

// Battery Management (ADC1 Channel 6 - Input only)
#define PIN_BATTERY_ADC         34
#define BATTERY_R1_OHMS         100000.0f  // 100k upper divider resistor
#define BATTERY_R2_OHMS         47000.0f   // 47k lower divider resistor
#define BATTERY_ADC_VREF        3.3f
#define BATTERY_ADC_RESOLUTION  4095.0f

// ============================================================================
// CONTROL LAW TUNABLES & SENSOR THRESHOLDS
// ============================================================================

// Discrete AQI Buckets (0 to 5) to Blower Target Duty Cycle (%)
// 0: Good (20%), 1: Moderate (35%), 2: USG (55%), 3: Unhealthy (75%), 4: Very Unhealthy (90%), 5: Hazardous (100%)
static const uint8_t AQI_BUCKET_TO_DUTY_PCT[6] = { 20, 35, 55, 75, 90, 100 };

// Safety Clamps & Operating Bounds
#define BLOWER_MIN_ACTIVE_DUTY   20      // Prevent motor stall when system is ON
#define BLOWER_MAX_AUTO_DUTY     80      // Auto ceiling to respect < 35 dB @ 1m noise limit
#define BLOWER_MAX_MANUAL_DUTY   100     // Maximum allowable manual duty cycle
#define BLOWER_FALLBACK_DUTY     50      // Fallback safe duty on sensor fault

// Slew-Rate Limiter
#define BLOWER_SLEW_STEP_PCT     5       // Max 5% change per 200 ms control tick (~25%/sec)
#define BLOWER_SLEW_INTERVAL_MS  200

// Manual Mode Presets (%)
#define MANUAL_DUTY_LOW          30
#define MANUAL_DUTY_MED          60
#define MANUAL_DUTY_HIGH         90

// ============================================================================
// TIMING CONSTANTS (MILLISECONDS)
// ============================================================================

#define SENSOR_WARMUP_TIME_MS    30000   // 30s PMS7003 laser diode stabilization
#define SENSOR_READ_INTERVAL_MS  1000    // Read cadence
#define SENSOR_OUTLET_OFFSET_MS  500     // Stagger ambient vs outlet reads
#define SENSOR_WATCHDOG_TIMEOUT  5000    // >5s without valid frame flags sensor fault

#define OLED_REFRESH_INTERVAL_MS 500     // 2 Hz display update to prevent I2C bus lag
#define BLE_NOTIFY_INTERVAL_MS   1000    // 1 Hz telemetry push to dashboard
#define BATTERY_READ_INTERVAL_MS 5000    // Read battery voltage every 5s
#define LOG_FLUSH_INTERVAL_MS    10000   // Write buffer to LittleFS CSV every 10s
#define LOW_BATTERY_ALERT_MS     30000   // Chirp buzzer every 30s when battery < 10%

// ============================================================================
// BLE GATT SERVICE & CHARACTERISTIC UUIDs (Random v4)
// ============================================================================

#define BLE_DEVICE_NAME          "SMART-AIR-SHIELD"
#define BLE_SERVICE_UUID         "1c7d24e0-32a1-4355-8e79-5e72d24260aa"
#define BLE_CHAR_TELEMETRY_UUID  "1c7d24e1-32a1-4355-8e79-5e72d24260aa"
#define BLE_CHAR_CONTROL_UUID    "1c7d24e2-32a1-4355-8e79-5e72d24260aa"

// ============================================================================
// FREERTOS TASK CONFIGURATION
// ============================================================================

#define TASK_SENSOR_STACK_SIZE   4096
#define TASK_CONTROL_STACK_SIZE  3072
#define TASK_UI_STACK_SIZE       4096
#define TASK_BLE_STACK_SIZE      4096

#define TASK_SENSOR_PRIORITY     3
#define TASK_CONTROL_PRIORITY    3
#define TASK_UI_PRIORITY         2
#define TASK_BLE_PRIORITY        1

#endif // SMART_AIR_SHIELD_CONFIG_H
