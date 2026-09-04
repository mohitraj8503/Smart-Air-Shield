#ifndef SMART_AIR_SHIELD_SESSION_LOG_H
#define SMART_AIR_SHIELD_SESSION_LOG_H

#include <stdint.h>
#include <stddef.h>

struct SessionRecord {
    uint32_t timestamp;
    uint16_t pm2_5_ambient;
    uint16_t pm10_ambient;
    uint16_t pm2_5_outlet;
    uint16_t pm10_outlet;
    uint16_t aqi;
    uint8_t  duty;
    uint8_t  battery;
    uint8_t  status;
};

class SessionLogger {
public:
    SessionLogger(size_t ramBufferSize = 60);

    bool begin();
    void logSample(const SessionRecord& record);
    void flush();
    size_t getLogSize() const;
    void clearLog();

    const char* getLogFilePath() const { return "/session.csv"; }

private:
    static const size_t MAX_BUFFER_SIZE = 60;
    SessionRecord _buffer[MAX_BUFFER_SIZE];
    size_t _head;
    size_t _count;
    size_t _capacity;
    bool _fsMounted;
};

#endif // SMART_AIR_SHIELD_SESSION_LOG_H
