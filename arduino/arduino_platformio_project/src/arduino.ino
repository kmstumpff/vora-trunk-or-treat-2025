#include <Arduino.h>
#include <ESP32Servo.h>
#include "BluetoothSerial.h"

// ========= CONFIG =========
static const int SERVO_PIN = 2;
static const uint32_t UNLOCK_PULSE_MS = 2000; // how long to "disconnect" IN
static const char *BT_NAME = "TN-ESP32-KMS";
// ==========================

BluetoothSerial SerialBT;
Servo candy_servo;

// Track our intended idle: coil ON (green LED ON)
static bool g_idleServoOpen = true;

static inline void setServoOpen(bool open)
{
  g_idleServoOpen = open;
  candy_servo.write(open ? 45 : 0); // 0 degrees = open, 45 degrees = closed
}

static void doUnlockPulseHiZ()
{
  setServoOpen(true);
  delay(UNLOCK_PULSE_MS);
  setServoOpen(false);
}

static void setupServo()
{
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  candy_servo.setPeriodHertz(50);            // standard 50 hz servo
  candy_servo.attach(SERVO_PIN, 1000, 2000); // attaches the servo on pin 2 to the servo object
}

static void handleCommand(String cmd)
{
  cmd.trim();
  cmd.toLowerCase();

  if (cmd == "open" || cmd == "pulse")
  {
    Serial.println("[OK] unlock pulse (hi-Z)");
    SerialBT.println("[OK] unlock pulse (hi-Z)");
    doUnlockPulseHiZ();
  }
  else if (cmd == "off" || cmd == "lock")
  {
    setServoOpen(false);
    Serial.println("[OK] locked (coil ON)");
    SerialBT.println("[OK] locked (coil ON)");
  }
  else if (cmd == "on" || cmd == "unlock")
  {
    setServoOpen(true);
    Serial.println("[OK] unlocked (coil OFF)");
    SerialBT.println("[OK] unlocked (coil OFF)");
  }
  else if (cmd == "status")
  {
    SerialBT.println(g_idleServoOpen ? "LOCKED" : "UNLOCKING");
  }
  else if (cmd.length() == 0)
  {
    // ignore
  }
  else
  {
    Serial.print("[ERR] invalid: ");
    Serial.println(cmd);
    SerialBT.println("[ERR] invalid");
  }
}

void setup()
{
  candy_servo.attach(SERVO_PIN);
  setServoOpen(false);

  Serial.begin(115200);
  Serial.println("Starting Bluetooth SPP…");
  if (!SerialBT.begin(BT_NAME))
  {
    Serial.println("BT start failed!");
    while (1)
      delay(1000);
  }
  Serial.printf("BT device name: %s\n", BT_NAME);
  Serial.println("Send: open|pulse  or  on|unlock  or  off|lock  or  float|hi-z  or  status");
}

void loop()
{
  static String serialbuf;
  static String btbuf;

  while (Serial.available())
  {
    char c = (char)Serial.read();
    Serial.write(c); // echo back to serial monitor
    if (c == '\r')
      continue;
    if (c == '\n')
    {
      handleCommand(serialbuf);
      serialbuf = "";
    }
    else
    {
      serialbuf += c;
      if (serialbuf.length() > 64)
        serialbuf = "";
    }
  }
  while (SerialBT.available())
  {
    char c = (char)SerialBT.read();
    if (c == '\r')
      continue;
    if (c == '\n')
    {
      handleCommand(btbuf);
      btbuf = "";
    }
    else
    {
      btbuf += c;
      if (btbuf.length() > 64)
        btbuf = "";
    }
  }
  delay(10);
}