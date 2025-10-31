#include <Arduino.h>
#include "LatchService/LatchService.h"
#include "configuration.h"

// Create latch instance
#if USE_DUMMY_LATCH
#include "Latch/DummyLatch.h"
DummyLatch latch;
#else
#include "Latch/ESPServoLatch.h"
ESPServoLatch latch(SERVO_PIN, 25);
#endif

// Create Latch service with the latch
LatchService latchService(&latch);

void setup() {
    Serial.begin(115200);
    delay(10000);
    
    Serial.println("\n=================================");
    Serial.println("ESP32 Latch Controller");
    Serial.println("=================================\n");
    
    // Initialize the latch hardware
    if (!latch.begin()) {
        Serial.println("[ERROR] Failed to initialize latch!");
        while (1) { delay(1000); }
    }

    // Initialize Latch service
    if (!latchService.begin(BT_NAME.c_str())) {
        Serial.println("[ERROR] Failed to initialize Latch service!");
        while (1) { delay(1000); }
    }
    
    Serial.println("\n=================================");
    Serial.println("Setup complete!");
    Serial.println("=================================\n");
}

void loop() {
    // Update Latch service
    latchService.update();

    // Add any other periodic tasks here
    delay(100);
}