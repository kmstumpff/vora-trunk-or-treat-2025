#pragma once

#include <Arduino.h>
#include <BLEUtils.h>
#include "Latch/ILatch.h"
#include "LatchService/ILatchService.h"

class BLELatchService: public ILatchService {
private:

    static constexpr const char* BLE_SERVICE_UUID = "dd3a359d-a0fb-49c2-9ba1-aae162aa2bdc";
    static constexpr const char* BLE_COMMAND_CHAR_UUID = "b1d3e840-56fb-41d8-9791-bd8566220d03";
    static constexpr const char* BLE_STATUS_CHAR_UUID = "7c6479dd-f6c6-4f68-8f1f-0cb38a06b1d3";
    static constexpr uint8_t BLE_CMD_OPEN = 0x01;
    static constexpr uint8_t BLE_CMD_CLOSE = 0x00;

    BLEServer* _server;
    BLEService* _service;
    BLECharacteristic* _commandChar;
    BLECharacteristic* _statusChar;
    
    // Callback for server events
    class ServerCallbacks : public BLEServerCallbacks {
    private:
        BLELatchService* _parent;
    public:
        ServerCallbacks(BLELatchService* parent);
        
        void onConnect(BLEServer* server) override;
        void onDisconnect(BLEServer* server) override;
    };
    
    // Callback for command characteristic
    class CommandCallbacks : public BLECharacteristicCallbacks {
    private:
        BLELatchService* _parent;
    public:
        CommandCallbacks(BLELatchService* parent);
        
        void onWrite(BLECharacteristic* characteristic) override;
    };
    
    void handleCommand(uint8_t cmd);
    void updateStatus();
    
public:
    BLELatchService(ILatch* latch);
    
    bool begin(const char* deviceName);
    void update();
};
