#include "session_log.h"
#include "../config.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#include <LittleFS.h>
#endif

SessionLogger::SessionLogger(size_t ramBufferSize)
    : _head(0),
      _count(0),
      _capacity((ramBufferSize > MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : ramBufferSize),
      _fsMounted(false)
{
}

bool SessionLogger::begin() {
#ifndef NATIVE_UNIT_TEST
    if (!LittleFS.begin(true)) {
        return false;
    }
    _fsMounted = true;

    // Check if session.csv exists; if not, write CSV header
    if (!LittleFS.exists("/session.csv")) {
        File file = LittleFS.open("/session.csv", FILE_WRITE);
        if (file) {
            file.println("timestamp_ms,pm25_amb,pm10_amb,pm25_out,pm10_out,aqi,duty_pct,battery_pct,status_flags");
            file.close();
        }
    }
#endif
    return true;
}

void SessionLogger::logSample(const SessionRecord& record) {
    _buffer[_head] = record;
    _head = (_head + 1) % _capacity;
    if (_count < _capacity) {
        _count++;
    }
}

void SessionLogger::flush() {
#ifndef NATIVE_UNIT_TEST
    if (!_fsMounted || _count == 0) return;

    File file = LittleFS.open("/session.csv", FILE_APPEND);
    if (!file) return;

    // Write all buffered records from oldest to newest
    size_t start = (_count == _capacity) ? _head : 0;
    for (size_t i = 0; i < _count; i++) {
        size_t idx = (start + i) % _capacity;
        const SessionRecord& r = _buffer[idx];

        file.printf("%u,%u,%u,%u,%u,%u,%u,%u,%u\n",
            r.timestamp,
            r.pm2_5_ambient,
            r.pm10_ambient,
            r.pm2_5_outlet,
            r.pm10_outlet,
            r.aqi,
            r.duty,
            r.battery,
            r.status
        );
    }

    file.close();
    _count = 0; // Clear RAM buffer after successful persistent write
#endif
}

size_t SessionLogger::getLogSize() const {
#ifndef NATIVE_UNIT_TEST
    if (!_fsMounted) return 0;
    if (LittleFS.exists("/session.csv")) {
        File file = LittleFS.open("/session.csv", FILE_READ);
        if (file) {
            size_t size = file.size();
            file.close();
            return size;
        }
    }
#endif
    return 0;
}

void SessionLogger::clearLog() {
#ifndef NATIVE_UNIT_TEST
    if (!_fsMounted) return;
    LittleFS.remove("/session.csv");
    File file = LittleFS.open("/session.csv", FILE_WRITE);
    if (file) {
        file.println("timestamp_ms,pm25_amb,pm10_amb,pm25_out,pm10_out,aqi,duty_pct,battery_pct,status_flags");
        file.close();
    }
    _count = 0;
    _head = 0;
#endif
}
