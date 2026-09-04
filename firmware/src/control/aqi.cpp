#include "aqi.h"
#include <math.h>

struct EPABreakpoint {
    float c_low;
    float c_high;
    int i_low;
    int i_high;
    AQIBucket bucket;
    const char* label;
};

static const EPABreakpoint EPA_BREAKPOINTS[] = {
    {   0.0f,  12.0f,   0,  50, AQI_BUCKET_GOOD,           "Good" },
    {  12.1f,  35.4f,  51, 100, AQI_BUCKET_MODERATE,       "Moderate" },
    {  35.5f,  55.4f, 101, 150, AQI_BUCKET_SENSITIVE,      "USG" },
    {  55.5f, 150.4f, 151, 200, AQI_BUCKET_UNHEALTHY,      "Unhealthy" },
    { 150.5f, 250.4f, 201, 300, AQI_BUCKET_VERY_UNHEALTHY, "Very Unhealthy" },
    { 250.5f, 350.4f, 301, 400, AQI_BUCKET_HAZARDOUS,      "Hazardous" },
    { 350.5f, 500.4f, 401, 500, AQI_BUCKET_HAZARDOUS,      "Hazardous" }
};

static const int NUM_BREAKPOINTS = sizeof(EPA_BREAKPOINTS) / sizeof(EPA_BREAKPOINTS[0]);

int pm25_to_aqi_bucket(float pm25) {
    if (pm25 < 0.0f) {
        return AQI_BUCKET_GOOD;
    }
    if (pm25 <= 12.0f) {
        return AQI_BUCKET_GOOD;
    }
    if (pm25 <= 35.4f) {
        return AQI_BUCKET_MODERATE;
    }
    if (pm25 <= 55.4f) {
        return AQI_BUCKET_SENSITIVE;
    }
    if (pm25 <= 150.4f) {
        return AQI_BUCKET_UNHEALTHY;
    }
    if (pm25 <= 250.4f) {
        return AQI_BUCKET_VERY_UNHEALTHY;
    }
    return AQI_BUCKET_HAZARDOUS;
}

int pm25_to_aqi_value(float pm25) {
    if (pm25 <= 0.0f) {
        return 0;
    }

    for (int i = 0; i < NUM_BREAKPOINTS; i++) {
        const EPABreakpoint& bp = EPA_BREAKPOINTS[i];
        if (pm25 >= bp.c_low && pm25 <= bp.c_high) {
            float slope = (float)(bp.i_high - bp.i_low) / (bp.c_high - bp.c_low);
            float aqi = slope * (pm25 - bp.c_low) + (float)bp.i_low;
            return (int)roundf(aqi);
        }
    }

    // Above 500.4 ug/m3: clamp to 500 or extrapolate with the top band slope
    if (pm25 > 500.4f) {
        return 500;
    }

    return 0;
}

const char* aqi_bucket_to_category_str(int bucket) {
    switch (bucket) {
        case AQI_BUCKET_GOOD:           return "Good";
        case AQI_BUCKET_MODERATE:       return "Moderate";
        case AQI_BUCKET_SENSITIVE:      return "Sensitive";
        case AQI_BUCKET_UNHEALTHY:      return "Unhealthy";
        case AQI_BUCKET_VERY_UNHEALTHY: return "Very Unhealthy";
        case AQI_BUCKET_HAZARDOUS:      return "Hazardous";
        default:                        return "Unknown";
    }
}

const char* aqi_to_category_str(int aqi) {
    if (aqi <= 50)  return "Good";
    if (aqi <= 100) return "Moderate";
    if (aqi <= 150) return "Sensitive";
    if (aqi <= 200) return "Unhealthy";
    if (aqi <= 300) return "Very Unhealthy";
    return "Hazardous";
}
