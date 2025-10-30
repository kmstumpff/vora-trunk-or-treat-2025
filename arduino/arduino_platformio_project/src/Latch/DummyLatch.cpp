#include <Arduino.h>
#include "Latch/DummyLatch.h"
#include "constants.h"

bool DummyLatch::begin() {
    Serial.println("[DummyLatch] Initialized");
    setOpen(false);
    return true;
}

bool DummyLatch::open() {
    Serial.println("[DummyLatch] opening latch");
    setOpen(true);
    return true;
}

bool DummyLatch::close() {
    Serial.println("[DummyLatch] closing latch");
    setOpen(false);
    return true;
}