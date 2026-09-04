#include "ble_service.h"

#ifndef NATIVE_UNIT_TEST
#include <Arduino.h>
#include <NimBLEDevice.h>

static NimBLEServer* s_pServer = nullptr;
static NimBLECharacteristic* s_pTelemetryChar = nullptr;
static NimBLECharacteristic* s_pControlChar = nullptr;
static bool s_deviceConnected = false;
static BleCommandCallback s_commandCallback = nullptr;

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) override {
        s_deviceConnected = true;
    }

    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) override {
        s_deviceConnected = true;
    }

    void onDisconnect(NimBLEServer* pServer) override {
        s_deviceConnected = false;
        // Automatically restart advertising on disconnect for seamless phone reconnection
        NimBLEDevice::startAdvertising();
    }
};

class ControlCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) override {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0 && s_commandCallback != nullptr) {
            uint8_t cmd = (uint8_t)value[0];
            uint8_t param = (value.length() > 1) ? (uint8_t)value[1] : 0;
            s_commandCallback((BleCommandType)cmd, param);
        }
    }
};
#endif

BleServiceManager::BleServiceManager()
    : _initialized(false),
      _cmdCallback(nullptr)
{
}

bool BleServiceManager::begin(BleCommandCallback cmdCallback) {
    _cmdCallback = cmdCallback;

#ifndef NATIVE_UNIT_TEST
    s_commandCallback = cmdCallback;

    // Initialize NimBLE Device
    NimBLEDevice::init(BLE_DEVICE_NAME);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Max TX power for reliable helmet-to-phone range

    // Create GATT Server
    s_pServer = NimBLEDevice::createServer();
    s_pServer->setCallbacks(new ServerCallbacks());

    // Create Main GATT Service
    NimBLEService* pService = s_pServer->createService(BLE_SERVICE_UUID);

    // Create Telemetry Notify Characteristic
    s_pTelemetryChar = pService->createCharacteristic(
        BLE_CHAR_TELEMETRY_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    // Create Control Write Characteristic
    s_pControlChar = pService->createCharacteristic(
        BLE_CHAR_CONTROL_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    s_pControlChar->setCallbacks(new ControlCallbacks());

    // Start GATT Service
    pService->start();

    // Start Advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // Fast connection advertisement interval
    pAdvertising->setMaxPreferred(0x12);
    NimBLEDevice::startAdvertising();
#endif

    _initialized = true;
    return true;
}

void BleServiceManager::updateTelemetry(const TelemetryPacket& packet) {
#ifndef NATIVE_UNIT_TEST
    if (!_initialized || s_pTelemetryChar == nullptr) return;

    // Set binary characteristic value and notify subscribed BLE client
    s_pTelemetryChar->setValue((uint8_t*)&packet, sizeof(TelemetryPacket));
    if (s_deviceConnected) {
        s_pTelemetryChar->notify();
    }
#endif
}

bool BleServiceManager::isConnected() const {
#ifndef NATIVE_UNIT_TEST
    return s_deviceConnected;
#else
    return false;
#endif
}
