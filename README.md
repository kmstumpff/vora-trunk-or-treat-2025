# Vora Trunk-or-Treat 2025

A Bluetooth-controlled candy dispenser system for Halloween Trunk-or-Treat events, consisting of an Android mobile app, ESP32 microcontroller firmware, and testing utilities.

## Overview

This project enables remote control of a servo-actuated candy dispenser mechanism via Bluetooth. An Android app provides a simple toggle interface to lock/unlock or dispense candy, communicating with an ESP32 microcontroller that physically controls the servo motor.

## Project Structure

```
vora-trunk-or-treat-2025/
├── android/                    # Android mobile application
│   └── VoraTrunkOrTreat2025/  # Kotlin-based Android Studio project
├── arduino/                    # ESP32 firmware
│   └── arduino_platformio_project/  # PlatformIO project
└── python/                     # Testing utilities
    └── bluetooth_test.py      # Command-line Bluetooth tester
```

## Components

### Android App

- **Platform**: Android (Kotlin)
- **Features**:
  - Simple toggle switch UI for candy dispenser control
  - Bluetooth SPP (Serial Port Profile) connectivity
  - Automatic device connection on startup
  - Permission handling for Bluetooth on Android 12+
- **Commands**: Sends "on" or "off" commands to the ESP32

### ESP32 Firmware

- **Platform**: ESP32 with PlatformIO
- **Features**:
  - Bluetooth Serial Port Profile (SPP) server
  - Servo motor control (0-45 degrees)
  - Multiple command support (open, pulse, lock, unlock, status)
  - Serial and Bluetooth command input
- **Hardware**: Controls a servo motor on GPIO pin 2

### Python Test Script

- **Purpose**: Command-line testing tool for Bluetooth connectivity
- **Features**: Simple interactive Bluetooth client for sending test commands to the ESP32

## Hardware Requirements

- ESP32 development board
- Servo motor (compatible with 50Hz PWM)
- Android device with Bluetooth support

## Getting Started

### ESP32 Setup

1. Install PlatformIO
2. Navigate to `arduino/arduino_platformio_project/`
3. Update the Bluetooth device name in `src/arduino.ino` if desired
4. Build and upload: `platformio run --target upload`

### Android App Setup

1. Open `android/VoraTrunkOrTreat2025/` in Android Studio
2. Update the `DEVICE_ADDRESS` in `MainActivity.kt` to match your ESP32's MAC address
3. Build and install the APK on your Android device
4. Grant Bluetooth permissions when prompted

### Python Testing

1. Install PyBluez: `pip install pybluez`
2. Update the `bd_addr` in `bluetooth_test.py` with your ESP32's MAC address
3. Run: `python python/bluetooth_test.py`

## Commands

The ESP32 accepts the following commands via Bluetooth or Serial:

- `on` or `unlock` - Unlock/open the mechanism
- `off` or `lock` - Lock/close the mechanism
- `open` or `pulse` - Perform a timed unlock pulse (2 seconds)
- `status` - Report current state
