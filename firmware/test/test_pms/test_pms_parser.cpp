#include <unity.h>
#include <string.h>
#include "sensors/pms_sensor.cpp"
#include "config.h"

// Helper to generate a valid 32-byte PMS7003 frame with correct checksum
static void createValidPmsFrame(uint8_t* frame, uint16_t pm1, uint16_t pm25, uint16_t pm10) {
    memset(frame, 0, 32);

    frame[0] = 0x42; // Header 1
    frame[1] = 0x4D; // Header 2
    frame[2] = 0x00; // Frame length high
    frame[3] = 0x1C; // Frame length low (28 bytes)

    // Standard particle fields
    frame[4] = (pm1 >> 8) & 0xFF;
    frame[5] = pm1 & 0xFF;
    frame[6] = (pm25 >> 8) & 0xFF;
    frame[7] = pm25 & 0xFF;
    frame[8] = (pm10 >> 8) & 0xFF;
    frame[9] = pm10 & 0xFF;

    // Atmospheric fields
    frame[10] = (pm1 >> 8) & 0xFF;
    frame[11] = pm1 & 0xFF;
    frame[12] = (pm25 >> 8) & 0xFF;
    frame[13] = pm25 & 0xFF;
    frame[14] = (pm10 >> 8) & 0xFF;
    frame[15] = pm10 & 0xFF;

    // Compute checksum (sum of bytes 0 to 29)
    uint16_t checksum = 0;
    for (size_t i = 0; i < 30; i++) {
        checksum += frame[i];
    }

    frame[30] = (checksum >> 8) & 0xFF;
    frame[31] = checksum & 0xFF;
}

void test_pms_checksum_and_parsing_valid(void) {
    uint8_t frame[32];
    createValidPmsFrame(frame, 15, 38, 72);

    // 1. Verify checksum validation passes
    TEST_ASSERT_TRUE(PMS7003Sensor::verifyChecksum(frame, 32));

    // 2. Parse frame and assert fields
    PMReading reading;
    bool success = PMS7003Sensor::parseBuffer(frame, reading, 1000);
    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL(15, reading.pm1_0);
    TEST_ASSERT_EQUAL(38, reading.pm2_5);
    TEST_ASSERT_EQUAL(72, reading.pm10);
    TEST_ASSERT_EQUAL(15, reading.pm1_0_atm);
    TEST_ASSERT_EQUAL(38, reading.pm2_5_atm);
    TEST_ASSERT_EQUAL(72, reading.pm10_atm);
    TEST_ASSERT_TRUE(reading.valid);
}

void test_pms_corrupted_checksum_rejected(void) {
    uint8_t frame[32];
    createValidPmsFrame(frame, 20, 50, 90);

    // Deliberately corrupt a single payload bit in PM2.5 byte
    frame[7] ^= 0x01;

    // Must be rejected by verifyChecksum
    TEST_ASSERT_FALSE(PMS7003Sensor::verifyChecksum(frame, 32));

    // Must be rejected by parseBuffer
    PMReading reading;
    bool success = PMS7003Sensor::parseBuffer(frame, reading, 1000);
    TEST_ASSERT_FALSE(success);
}

void test_pms_invalid_preamble_rejected(void) {
    uint8_t frame[32];
    createValidPmsFrame(frame, 10, 20, 30);

    // Corrupt first start byte
    frame[0] = 0x41;
    TEST_ASSERT_FALSE(PMS7003Sensor::verifyChecksum(frame, 32));

    // Corrupt second start byte
    frame[0] = 0x42;
    frame[1] = 0x4E;
    TEST_ASSERT_FALSE(PMS7003Sensor::verifyChecksum(frame, 32));
}

void test_pms_streaming_byte_state_machine(void) {
    PMS7003Sensor sensor;
    uint8_t frame[32];
    createValidPmsFrame(frame, 8, 22, 45);

    // Initialize sensor with simulated start at timestamp 1000ms
    uint32_t t_start = 1000;

    // Send random noise before the frame at start
    uint8_t noise[] = { 0xAA, 0x55, 0x42, 0x12, 0x42 }; // Contains partial false sync 0x42
    for (size_t i = 0; i < sizeof(noise); i++) {
        TEST_ASSERT_FALSE(sensor.processByte(noise[i], t_start));
    }

    // Advance time past the 30s warmup period
    uint32_t t_warmed = t_start + SENSOR_WARMUP_TIME_MS + 1000;

    // Now feed the valid 32-byte frame byte-by-byte
    bool completed = false;
    for (size_t i = 0; i < 32; i++) {
        bool res = sensor.processByte(frame[i], t_warmed);
        if (i == 31) {
            completed = res;
        } else {
            TEST_ASSERT_FALSE(res);
        }
    }

    TEST_ASSERT_TRUE(completed);
    const PMReading& latest = sensor.getLatestReading();
    TEST_ASSERT_EQUAL(8, latest.pm1_0);
    TEST_ASSERT_EQUAL(22, latest.pm2_5);
    TEST_ASSERT_EQUAL(45, latest.pm10);
    TEST_ASSERT_TRUE(latest.valid);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_pms_checksum_and_parsing_valid);
    RUN_TEST(test_pms_corrupted_checksum_rejected);
    RUN_TEST(test_pms_invalid_preamble_rejected);
    RUN_TEST(test_pms_streaming_byte_state_machine);
    return UNITY_END();
}
