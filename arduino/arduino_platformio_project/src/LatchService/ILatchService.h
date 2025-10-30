#pragma once


#include "Latch/ILatch.h"


class ILatchService {
private:
    
    ILatch* _latch;
    bool _isConnected;


protected:
    void setConnected(bool connected) {
        _isConnected = connected;
    }

    ILatch* getLatch() const {
        return _latch;
    }
    
public:
    ILatchService(ILatch* latch) 
        : _latch(latch), _isConnected(false) {}
    
    virtual bool begin(const char* deviceName = "ESP32-Latch") = 0;

    virtual void update() = 0;

    bool isConnected() const {
        return _isConnected;
    }
};
