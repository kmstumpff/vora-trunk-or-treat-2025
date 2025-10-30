#include <Arduino.h>
#include "LatchService/BTClassicLatchService.h"
#include <BluetoothSerial.h>
#include "constants.h"

BTClassicLatchService::BTClassicLatchService(ILatch* latch): ILatchService(latch) {}

void BTClassicLatchService::handleCommand(String cmd) {
    cmd.trim();
    cmd.toLowerCase();

    if (cmd == "off" || cmd == "lock")
    {
        this->getLatch()->close();
        Serial.println("[OK] locked");
    }
    else if (cmd == "on" || cmd == "unlock")
    {
        this->getLatch()->open();
        Serial.println("[OK] unlocked");
    }
    else if (cmd == "status")
    {
        SerialBT.println(this->getLatch()->isOpen() ? "LOCKED" : "UNLOCKED");
    }
    else if (cmd.length() == 0)
    {
        // ignore
    }
    else
    {
        Serial.print("[ERR] invalid: ");
        Serial.println(cmd);
    }
}
    
void BTClassicLatchService::updateStatus() {
    uint8_t status = this->getLatch()->isOpen();
    if (isConnected()) {
        if (status) {
            SerialBT.println("status: open");
        } else {
            SerialBT.println("status: closed");
        }
    }
}

bool BTClassicLatchService::begin(const char* deviceName) {
    bool result = SerialBT.begin(deviceName);
    // TODO actually check if bt is connected?
    this->setConnected(result);
    return result;
}

void BTClassicLatchService::update() {
    // clear buffer if no activity for 5 seconds
    if (_buffer.length() > 0 && millis() - _lastBTUpdate > 5000) {
        _buffer = "";
    }


    while (SerialBT.available())
    {
        char c = (char)SerialBT.read();
        if (c == '\r')
        continue;
        if (c == '\n')
        {
            handleCommand(_buffer);
            _buffer = "";
        }
        else
        {
            _buffer += c;
            if (_buffer.length() > 64)
                _buffer = "";
        }
        _lastBTUpdate = millis();
    } 

    // handle serial debug input
    while (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'o':
                debug("[DEBUG] Sending OPEN command\n");
                handleCommand("on");
                break;
            case 'c':
                debug("[DEBUG] Sending CLOSE command\n");
                handleCommand("off");
                break;
            default:
                debug("[DEBUG] Unknown debug command: %c\n", cmd);
                break;
        }
    }
}
