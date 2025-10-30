#pragma once

#include <Arduino.h>

// #define _DEBUG                    // Set debug mode, results in more verbose output on serial port
#include "constants.h"

#define USE_BLE true // set to 'true' to use BLE instead of Classic SPP
#define USE_DUMMY_LATCH true // set to 'true' to use DummyLatch for testing


static const String BT_NAME = "TN-ESP32";

// #if BOARD == BOARD_ESP32_DEVKITV1
static const int SERVO_PIN = 2; // ESP32 DevKitV1 servo pin
// #endif
