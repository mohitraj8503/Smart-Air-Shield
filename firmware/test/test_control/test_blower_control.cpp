#include <unity.h>
#include "control/aqi.cpp"
#include "control/blower_control.cpp"
#include "config.h"

void test_slew_rate_step(void) {
    // Ramp up test with step = 5
    TEST_ASSERT_EQUAL(25, BlowerController::computeSlewStep(20, 50, 5));
    TEST_ASSERT_EQUAL(30, BlowerController::computeSlewStep(25, 50, 5));
    TEST_ASSERT_EQUAL(50, BlowerController::computeSlewStep(48, 50, 5)); // Clamps to target without overshooting

    // Ramp down test with step = 5
    TEST_ASSERT_EQUAL(45, BlowerController::computeSlewStep(50, 20, 5));
    TEST_ASSERT_EQUAL(40, BlowerController::computeSlewStep(45, 20, 5));
    TEST_ASSERT_EQUAL(20, BlowerController::computeSlewStep(22, 20, 5)); // Clamps to target without undershooting

    // Exact match
    TEST_ASSERT_EQUAL(35, BlowerController::computeSlewStep(35, 35, 5));
}

void test_bucket_mapping_and_ceiling(void) {
    BlowerController blower;
    blower.begin(PIN_BLOWER_PWM);
    blower.setMode(MODE_AUTO);

    // Bucket 0 (Good) -> Target 20%
    blower.update(0, false);
    TEST_ASSERT_EQUAL(20, blower.getTargetDuty());

    // Bucket 1 (Moderate) -> Target 35%
    blower.update(1, false);
    TEST_ASSERT_EQUAL(35, blower.getTargetDuty());

    // Bucket 2 (Sensitive) -> Target 55%
    blower.update(2, false);
    TEST_ASSERT_EQUAL(55, blower.getTargetDuty());

    // Bucket 3 (Unhealthy) -> Target 75%
    blower.update(3, false);
    TEST_ASSERT_EQUAL(75, blower.getTargetDuty());

    // Bucket 4 (Very Unhealthy): Raw 90%, but clamped to Auto ceiling (80%) for noise < 35 dB @ 1m
    blower.update(4, false);
    TEST_ASSERT_EQUAL(BLOWER_MAX_AUTO_DUTY, blower.getTargetDuty());
    TEST_ASSERT_EQUAL(80, blower.getTargetDuty());

    // Bucket 5 (Hazardous): Raw 100%, clamped to 80% in Auto
    blower.update(5, false);
    TEST_ASSERT_EQUAL(BLOWER_MAX_AUTO_DUTY, blower.getTargetDuty());
}

void test_slew_rate_ramp_over_ticks(void) {
    BlowerController blower;
    blower.begin(PIN_BLOWER_PWM);
    blower.setMode(MODE_AUTO);

    // Starting from 0 duty, target becomes 75% (bucket 3)
    // Ticks should advance by +5 each tick
    uint8_t d1 = blower.update(3, false);
    TEST_ASSERT_EQUAL(5, d1);

    uint8_t d2 = blower.update(3, false);
    TEST_ASSERT_EQUAL(10, d2);

    uint8_t d3 = blower.update(3, false);
    TEST_ASSERT_EQUAL(15, d3);

    uint8_t d4 = blower.update(3, false);
    TEST_ASSERT_EQUAL(20, d4);
}

void test_manual_mode_and_presets(void) {
    BlowerController blower;
    blower.begin(PIN_BLOWER_PWM);

    // Manual duty set
    blower.setManualDuty(65);
    TEST_ASSERT_EQUAL(MODE_MANUAL, blower.getMode());
    TEST_ASSERT_EQUAL(65, blower.getManualDuty());

    // Preset cycling
    blower.setMode(MODE_AUTO);
    blower.cycleManualPreset(); // Auto -> Manual Low (30%)
    TEST_ASSERT_EQUAL(MODE_MANUAL, blower.getMode());
    TEST_ASSERT_EQUAL(MANUAL_DUTY_LOW, blower.getManualDuty());

    blower.cycleManualPreset(); // Manual Low -> Med (60%)
    TEST_ASSERT_EQUAL(MANUAL_DUTY_MED, blower.getManualDuty());

    blower.cycleManualPreset(); // Manual Med -> High (90%)
    TEST_ASSERT_EQUAL(MANUAL_DUTY_HIGH, blower.getManualDuty());

    blower.cycleManualPreset(); // Manual High -> Auto
    TEST_ASSERT_EQUAL(MODE_AUTO, blower.getMode());
}

void test_sensor_fault_fallback(void) {
    BlowerController blower;
    blower.begin(PIN_BLOWER_PWM);
    blower.setMode(MODE_AUTO);

    // When sensor fault is flagged, blower targets safe medium duty (50%)
    blower.update(0, true);
    TEST_ASSERT_EQUAL(BLOWER_FALLBACK_DUTY, blower.getTargetDuty());
    TEST_ASSERT_EQUAL(50, blower.getTargetDuty());
}

void test_power_toggle(void) {
    BlowerController blower;
    blower.begin(PIN_BLOWER_PWM);
    blower.setMode(MODE_AUTO);

    TEST_ASSERT_TRUE(blower.isPoweredOn());

    blower.togglePower(); // Turn OFF
    TEST_ASSERT_FALSE(blower.isPoweredOn());
    TEST_ASSERT_EQUAL(MODE_OFF, blower.getMode());
    blower.update(3, false);
    TEST_ASSERT_EQUAL(0, blower.getTargetDuty());

    blower.togglePower(); // Turn back ON -> restores MODE_AUTO
    TEST_ASSERT_TRUE(blower.isPoweredOn());
    TEST_ASSERT_EQUAL(MODE_AUTO, blower.getMode());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_slew_rate_step);
    RUN_TEST(test_bucket_mapping_and_ceiling);
    RUN_TEST(test_slew_rate_ramp_over_ticks);
    RUN_TEST(test_manual_mode_and_presets);
    RUN_TEST(test_sensor_fault_fallback);
    RUN_TEST(test_power_toggle);
    return UNITY_END();
}
