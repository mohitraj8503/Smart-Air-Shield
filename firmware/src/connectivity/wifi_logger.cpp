#include "wifi_logger.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

static WebServer s_server(80);
static TelemetryPacket s_latestPacket;

static void handleRoot() {
    String html = "<!DOCTYPE html><html><head><title>SMART AIR-SHIELD</title>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<style>body{font-family:sans-serif;padding:20px;max-width:500px;margin:auto;background:#0f172a;color:#f8fafc}";
    html += ".card{background:#1e293b;padding:16px;border-radius:12px;margin-bottom:16px}";
    html += ".btn{display:inline-block;padding:10px 20px;background:#38bdf8;color:#0f172a;text-decoration:none;border-radius:8px;font-weight:bold}";
    html += "</style></head><body>";
    html += "<h2>SMART AIR-SHIELD</h2>";
    html += "<p>Helmet Air Purification Telemetry & Log Export</p>";
    html += "<div class='card'>";
    html += "<p><b>AQI:</b> " + String(s_latestPacket.aqi_value) + "</p>";
    html += "<p><b>Ambient PM2.5:</b> " + String(s_latestPacket.pm2_5_ambient) + " ug/m3</p>";
    html += "<p><b>Outlet PM2.5:</b> " + String(s_latestPacket.pm2_5_outlet) + " ug/m3</p>";
    html += "<p><b>Blower Duty:</b> " + String(s_latestPacket.blower_duty) + "%</p>";
    html += "<p><b>Battery:</b> " + String(s_latestPacket.battery_pct) + "%</p>";
    html += "</div>";
    html += "<a href='/log.csv' class='btn'>Download Session CSV</a>";
    html += "</body></html>";
    s_server.send(200, "text/html", html);
}

static void handleLogDownload() {
    if (LittleFS.exists("/session.csv")) {
        File file = LittleFS.open("/session.csv", FILE_READ);
        s_server.streamFile(file, "text/csv");
        file.close();
    } else {
        s_server.send(404, "text/plain", "Log file not found");
    }
}

static void handleStatusJson() {
    StaticJsonDocument<256> doc;
    doc["timestamp"] = s_latestPacket.timestamp;
    doc["aqi"] = s_latestPacket.aqi_value;
    doc["bucket"] = s_latestPacket.aqi_bucket;
    doc["pm25_amb"] = s_latestPacket.pm2_5_ambient;
    doc["pm10_amb"] = s_latestPacket.pm10_ambient;
    doc["pm25_out"] = s_latestPacket.pm2_5_outlet;
    doc["pm10_out"] = s_latestPacket.pm10_outlet;
    doc["duty"] = s_latestPacket.blower_duty;
    doc["battery"] = s_latestPacket.battery_pct;
    doc["mode"] = s_latestPacket.operating_mode;
    doc["status"] = s_latestPacket.status_flags;

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    s_server.send(200, "application/json", jsonResponse);
}
#endif

WifiLogger::WifiLogger()
    : _running(false)
{
    _latestPacket = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

bool WifiLogger::startAccessPoint(const char* ssid, const char* password) {
#ifndef NATIVE_UNIT_TEST
    WiFi.mode(WIFI_AP);
    bool apStarted = WiFi.softAP(ssid, password);
    if (!apStarted) {
        return false;
    }

    s_server.on("/", HTTP_GET, handleRoot);
    s_server.on("/log.csv", HTTP_GET, handleLogDownload);
    s_server.on("/status", HTTP_GET, handleStatusJson);
    s_server.begin();

    _running = true;
    return true;
#else
    return false;
#endif
}

void WifiLogger::stop() {
#ifndef NATIVE_UNIT_TEST
    if (_running) {
        s_server.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        _running = false;
    }
#endif
}

void WifiLogger::handleClient() {
#ifndef NATIVE_UNIT_TEST
    if (_running) {
        s_server.handleClient();
    }
#endif
}

void WifiLogger::updateTelemetry(const TelemetryPacket& packet) {
    _latestPacket = packet;
#ifndef NATIVE_UNIT_TEST
    s_latestPacket = packet;
#endif
}
