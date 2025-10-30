#include "Latch/ESPServoLatch.h"
#include <ESP32Servo.h>
#include "constants.h"

ESPServoLatch::ESPServoLatch(int pin, int hertz) : ILatch(), _pin(pin), _hertz(hertz) {}

bool ESPServoLatch::begin() {
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    _servo.setPeriodHertz(_hertz); // 25 hz servo
    _servo.attach(_pin); // attaches the servo on pin 2 to the servo object
    close(); // Start closed
    debug("[ESPServoLatch] Initialized\n");
    
    return true;
}

bool ESPServoLatch::open() {
    debug("[ESPServoLatch] opening latch\n");
    _servo.write(90);
    setOpen(true);
    return true;
}

bool ESPServoLatch::close() {
    debug("[ESPServoLatch] closing latch\n");
    _servo.write(0);
    setOpen(false);
    return true;
}
