#include <unity.h>
#include "control/aqi.cpp"

void test_aqi_bucket_boundaries(void) {
    // Bucket 0: Good (0 - 12.0 ug/m3)
    TEST_ASSERT_EQUAL(AQI_BUCKET_GOOD, pm25_to_aqi_bucket(0.0f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_GOOD, pm25_to_aqi_bucket(6.5f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_GOOD, pm25_to_aqi_bucket(12.0f));

    // Bucket 1: Moderate (12.1 - 35.4 ug/m3)
    TEST_ASSERT_EQUAL(AQI_BUCKET_MODERATE, pm25_to_aqi_bucket(12.1f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_MODERATE, pm25_to_aqi_bucket(25.0f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_MODERATE, pm25_to_aqi_bucket(35.4f));

    // Bucket 2: Unhealthy for Sensitive Groups (35.5 - 55.4 ug/m3)
    TEST_ASSERT_EQUAL(AQI_BUCKET_SENSITIVE, pm25_to_aqi_bucket(35.5f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_SENSITIVE, pm25_to_aqi_bucket(45.0f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_SENSITIVE, pm25_to_aqi_bucket(55.4f));

    // Bucket 3: Unhealthy (55.5 - 150.4 ug/m3)
    TEST_ASSERT_EQUAL(AQI_BUCKET_UNHEALTHY, pm25_to_aqi_bucket(55.5f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_UNHEALTHY, pm25_to_aqi_bucket(100.0f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_UNHEALTHY, pm25_to_aqi_bucket(150.4f));

    // Bucket 4: Very Unhealthy (150.5 - 250.4 ug/m3)
    TEST_ASSERT_EQUAL(AQI_BUCKET_VERY_UNHEALTHY, pm25_to_aqi_bucket(150.5f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_VERY_UNHEALTHY, pm25_to_aqi_bucket(200.0f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_VERY_UNHEALTHY, pm25_to_aqi_bucket(250.4f));

    // Bucket 5: Hazardous (> 250.4 ug/m3)
    TEST_ASSERT_EQUAL(AQI_BUCKET_HAZARDOUS, pm25_to_aqi_bucket(250.5f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_HAZARDOUS, pm25_to_aqi_bucket(380.0f));
    TEST_ASSERT_EQUAL(AQI_BUCKET_HAZARDOUS, pm25_to_aqi_bucket(600.0f));
}

void test_aqi_value_linear_interpolation(void) {
    // 0 ug/m3 -> AQI 0
    TEST_ASSERT_EQUAL(0, pm25_to_aqi_value(0.0f));

    // 12.0 ug/m3 -> AQI 50 (Boundary of Good)
    TEST_ASSERT_EQUAL(50, pm25_to_aqi_value(12.0f));

    // 12.1 ug/m3 -> AQI 51 (Start of Moderate)
    TEST_ASSERT_EQUAL(51, pm25_to_aqi_value(12.1f));

    // 35.4 ug/m3 -> AQI 100 (End of Moderate)
    TEST_ASSERT_EQUAL(100, pm25_to_aqi_value(35.4f));

    // 55.4 ug/m3 -> AQI 150 (End of Sensitive)
    TEST_ASSERT_EQUAL(150, pm25_to_aqi_value(55.4f));

    // 150.4 ug/m3 -> AQI 200 (End of Unhealthy)
    TEST_ASSERT_EQUAL(200, pm25_to_aqi_value(150.4f));

    // 250.4 ug/m3 -> AQI 300 (End of Very Unhealthy)
    TEST_ASSERT_EQUAL(300, pm25_to_aqi_value(250.4f));

    // Midpoint check in Moderate band: (12.1 + 35.4) / 2 = 23.75 -> AQI ~75-76
    int midAqi = pm25_to_aqi_value(23.75f);
    TEST_ASSERT_TRUE(midAqi >= 74 && midAqi <= 77);
}

void test_aqi_category_strings(void) {
    TEST_ASSERT_EQUAL_STRING("Good", aqi_bucket_to_category_str(AQI_BUCKET_GOOD));
    TEST_ASSERT_EQUAL_STRING("Moderate", aqi_bucket_to_category_str(AQI_BUCKET_MODERATE));
    TEST_ASSERT_EQUAL_STRING("Sensitive", aqi_bucket_to_category_str(AQI_BUCKET_SENSITIVE));
    TEST_ASSERT_EQUAL_STRING("Unhealthy", aqi_bucket_to_category_str(AQI_BUCKET_UNHEALTHY));
    TEST_ASSERT_EQUAL_STRING("Very Unhealthy", aqi_bucket_to_category_str(AQI_BUCKET_VERY_UNHEALTHY));
    TEST_ASSERT_EQUAL_STRING("Hazardous", aqi_bucket_to_category_str(AQI_BUCKET_HAZARDOUS));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_aqi_bucket_boundaries);
    RUN_TEST(test_aqi_value_linear_interpolation);
    RUN_TEST(test_aqi_category_strings);
    return UNITY_END();
}
