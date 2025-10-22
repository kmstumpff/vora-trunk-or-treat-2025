#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
String btCommand = "";
unsigned long lastCharTime = 0;
const unsigned long COMMAND_TIMEOUT = 2000; // 2 seconds

int RELAY_PIN = 2;

void setup() {
    Serial.begin(115200);
    SerialBT.begin("TN-ESP32");

    pinMode(RELAY_PIN, OUTPUT);
}

void handleCommand(const String& input) {
    String command = input;
    command.trim();
    if (command == "on") {
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("Relay turned ON");
    } else if (command == "off") {
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("Relay turned OFF");
    } else {
        Serial.println("Error: Invalid command");
    }
}

void loop() {
    if (Serial.available()) {
        SerialBT.write(Serial.read());
    }
    while (SerialBT.available()) {
        char c = SerialBT.read();
        Serial.write(c);
        btCommand += c;
        lastCharTime = millis();

        if (c == '\n') {
            handleCommand(btCommand);
            btCommand = ""; // Reset for next command
        }
    }

    // Timeout handling
    if (btCommand.length() > 0 && (millis() - lastCharTime > COMMAND_TIMEOUT)) {
        Serial.println("Error: Command timeout");
        btCommand = "";
    }

    delay(25);
}
