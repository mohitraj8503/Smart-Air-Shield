#ifndef SMART_AIR_SHIELD_WIFI_LOGGER_H
#define SMART_AIR_SHIELD_WIFI_LOGGER_H

#include <stdint.h>
#include "ble_service.h"

class WifiLogger {
public:
    WifiLogger();

    // Starts SoftAP mode ("SMART-AIR-SHIELD-WIFI", open/pwd) and HTTP server on port 80
    bool startAccessPoint(const char* ssid = "SMART-AIR-SHIELD", const char* password = nullptr);
    void stop();

    // Handle incoming client HTTP requests
    void handleClient();

    bool isRunning() const { return _running; }

    // Update latest telemetry for /status JSON endpoint
    void updateTelemetry(const TelemetryPacket& packet);

private:
    bool _running;
    TelemetryPacket _latestPacket;
};

#endif // SMART_AIR_SHIELD_WIFI_LOGGER_H
