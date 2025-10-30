#pragma once

#include "Latch/ILatch.h"

/**
 * @brief Example implementation of ILatch for testing
 * 
 * Replace this with your actual latch implementation
 * (servo, solenoid, relay, motor, etc.)
 */
class DummyLatch : public ILatch {
    
public:
    DummyLatch() : ILatch() {}

    virtual bool begin() override;
    virtual bool open() override;
    virtual bool close() override;
};
