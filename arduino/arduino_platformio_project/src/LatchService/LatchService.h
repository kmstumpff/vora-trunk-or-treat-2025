#pragma once



#include "configuration.h"
#include "LatchService/ILatchService.h"

#if USE_BLE
#include "LatchService/BLELatchService.h"
#else
#include "LatchService/BTClassicLatchService.h"
#endif

#include "Latch/ILatch.h"


class LatchService{
private:
    ILatchService *_baseService;
    
    
public:
    LatchService(ILatch* latch) {
#if USE_BLE
        _baseService = new BLELatchService(latch);
#else
        _baseService = new BTClassicLatchService(latch);
#endif

    }
    
    inline bool begin(const char* deviceName = "ESP32-Latch") {
        return _baseService->begin(deviceName);
    }
    
    inline void update() {
        _baseService->update();
    }
};
