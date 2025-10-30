#pragma once

#include <Arduino.h>
#include "Latch/ILatch.h"
#include "LatchService/ILatchService.h"
#include <BluetoothSerial.h>

class BTClassicLatchService: public ILatchService {
private:
    // Command values
    
    BluetoothSerial SerialBT;
    String _buffer;
    unsigned long _lastBTUpdate = 0;
    
    void handleCommand(String cmd);
    void updateStatus();
    
public:
    BTClassicLatchService(ILatch* latch);
    
    bool begin(const char* deviceName);
    void update();
};
