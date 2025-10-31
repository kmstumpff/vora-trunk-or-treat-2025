#pragma once

// #define _DEBUG  // Set debug mode, results in more verbose output on serial port

// Boards
#define BOARD_NRF52_FEATHER 0
#define BOARD_ESP32_DEVKITV1 1


#if defined(ARDUINO_NRF52_ADAFRUIT)
#define BOARD              BOARD_NRF52_FEATHER
// #elif defined(ARDUINO_ESP32_ADAFRUIT)
// #define BOARD              BOARD_ESP32_FEATHER
// #elif defined(LOLIN_D32)
// #define BOARD              BOARD_ESP32_LOLIND32
#elif defined(ARDUINO_ESP32_DEV)
#define BOARD              BOARD_ESP32_DEVKITV1
#endif


#ifdef _DEBUG
  #define debug(fmt, ...) { Serial.printf(fmt,## __VA_ARGS__); }
#else
  #define debug(fmt, ...) {} // Silence is golden
#endif
