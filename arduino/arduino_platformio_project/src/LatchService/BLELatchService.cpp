#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "LatchService/BLELatchService.h"
#include "constants.h"


void BLELatchService::handleCommand(uint8_t cmd) {
    bool success = false;
    
    switch (cmd) {
        case BLE_CMD_OPEN:
            debug("[BLE] Received OPEN command\n");
            success = this->getLatch()->open();
            break;
        case BLE_CMD_CLOSE:
            debug("[BLE] Received CLOSE command\n");
            success = this->getLatch()->close();
            break;
        default:
            Serial.printf("[BLE] Unknown command: 0x%02X\n", cmd);
            return;
    }
    
    if (success) {
        updateStatus();
    } else {
        Serial.println("[BLE] Command execution failed");
    }
}

void BLELatchService::updateStatus() {
    uint8_t status = this->getLatch()->isOpen() ? 0x01 : 0x00;
    _statusChar->setValue(&status, 1);
    if (isConnected()) {
        _statusChar->notify();
    }
}

BLELatchService::BLELatchService(ILatch* latch) 
    : ILatchService(latch), _server(nullptr), _service(nullptr),
        _commandChar(nullptr), _statusChar(nullptr) {}

bool BLELatchService::begin(const char* deviceName) {
    // Initialize BLE
    BLEDevice::init(deviceName);
    
    // Create BLE Server
    _server = BLEDevice::createServer();
    _server->setCallbacks(new ServerCallbacks(this));
    
    // Create BLE Service
    _service = _server->createService(BLE_SERVICE_UUID);
    
    // Create Command Characteristic (Write)
    _commandChar = _service->createCharacteristic(
        BLE_COMMAND_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    _commandChar->setCallbacks(new CommandCallbacks(this));
    
    // Create Status Characteristic (Read + Notify)
    _statusChar = _service->createCharacteristic(
        BLE_STATUS_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    _statusChar->addDescriptor(new BLE2902());
    
    // Set initial status
    uint8_t initialStatus = this->getLatch()->isOpen() ? 0x01 : 0x00;
    _statusChar->setValue(&initialStatus, 1);
    
    // Start the service
    _service->start();
    
    // Start advertising
    BLEAdvertising* advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(BLE_SERVICE_UUID);
    advertising->setScanResponse(true);
    advertising->setMinPreferred(0x06);  // Functions that help with iPhone connections
    advertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    
    Serial.println("[BLE] Service started");
    debug("[BLE] Device name: %s\n", deviceName);
    debug("[BLE] Service UUID: %s\n", BLE_SERVICE_UUID);
    Serial.println("[BLE] Waiting for connections...");
    
    return true;
}

void BLELatchService::update() {
    // Periodically update status if connected
    if (isConnected()) {
        static unsigned long lastUpdate = 0;
        if (millis() - lastUpdate > 5000) { // Update every 5 seconds
            updateStatus();
            lastUpdate = millis();
        }
    }

    // handle serial debug input
    while (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'o':
                debug("[DEBUG] Sending OPEN command\n");
                handleCommand(BLE_CMD_OPEN);
                break;
            case 'c':
                debug("[DEBUG] Sending CLOSE command\n");
                handleCommand(BLE_CMD_CLOSE);
                break;
            default:
                debug("[DEBUG] Unknown debug command: %c\n", cmd);
                break;
        }
    }
}

BLELatchService::ServerCallbacks::ServerCallbacks(BLELatchService* parent) : _parent(parent) {}

void BLELatchService::ServerCallbacks::onConnect(BLEServer* server) {
    _parent->setConnected(true);
    Serial.println("[BLE] Client connected");
}

void BLELatchService::ServerCallbacks::onDisconnect(BLEServer* server) {
    _parent->setConnected(false);
    Serial.println("[BLE] Client disconnected");
    // Restart advertising
    delay(500); // Give BLE stack time to clean up
    server->startAdvertising();
}

BLELatchService::CommandCallbacks::CommandCallbacks(BLELatchService* parent) : _parent(parent) {}
    
void BLELatchService::CommandCallbacks::onWrite(BLECharacteristic* characteristic) {
    std::string value = characteristic->getValue();
    if (value.length() > 0) {
        uint8_t cmd = value[0];
        _parent->handleCommand(cmd);
    }
}