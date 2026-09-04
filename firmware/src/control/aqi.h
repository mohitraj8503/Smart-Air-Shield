#ifndef SMART_AIR_SHIELD_AQI_H
#define SMART_AIR_SHIELD_AQI_H

#include <stdint.h>

/**
 * US EPA PM2.5 Breakpoints & AQI Calculation
 * 
 * Standard EPA Breakpoints:
 * Bucket 0: 0.0 - 12.0 ug/m3    -> Good (AQI 0 - 50)
 * Bucket 1: 12.1 - 35.4 ug/m3   -> Moderate (AQI 51 - 100)
 * Bucket 2: 35.5 - 55.4 ug/m3   -> Unhealthy for Sensitive Groups (AQI 101 - 150)
 * Bucket 3: 55.5 - 150.4 ug/m3  -> Unhealthy (AQI 151 - 200)
 * Bucket 4: 150.5 - 250.4 ug/m3 -> Very Unhealthy (AQI 201 - 300)
 * Bucket 5: 250.5+ ug/m3        -> Hazardous (AQI 301 - 500)
 */

enum AQIBucket {
    AQI_BUCKET_GOOD = 0,
    AQI_BUCKET_MODERATE = 1,
    AQI_BUCKET_SENSITIVE = 2,
    AQI_BUCKET_UNHEALTHY = 3,
    AQI_BUCKET_VERY_UNHEALTHY = 4,
    AQI_BUCKET_HAZARDOUS = 5
};

// Maps PM2.5 concentration (ug/m3) to discrete control law bucket (0 - 5)
int pm25_to_aqi_bucket(float pm25);

// Calculates continuous US EPA AQI index value (0 - 500+) via piecewise linear interpolation
int pm25_to_aqi_value(float pm25);

// Human-readable category label
const char* aqi_bucket_to_category_str(int bucket);
const char* aqi_to_category_str(int aqi);

#endif // SMART_AIR_SHIELD_AQI_H
