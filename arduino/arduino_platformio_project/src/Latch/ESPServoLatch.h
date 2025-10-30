#pragma once

#include "Latch/ILatch.h"
#include <ESP32Servo.h>

class ESPServoLatch : public ILatch {
private:
    Servo _servo;
    int _pin;
    int _hertz;
    
public:
    ESPServoLatch(int pin, int hertz = 50);
    
    virtual bool begin() override;
    virtual bool open() override;
    virtual bool close() override;
};
