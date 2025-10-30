#!/usr/bin/env python3
"""
BLE Latch Controller Test Script
Uses bleak to connect to ESP32 and control the latch
"""

import asyncio
from bleak import BleakClient, BleakScanner
import sys

DEFAULT_BLE_DEVICE_NAME = "TN-ESP32-KMS"
DEFAULT_BLE_DEVICE_ADDRESS = "24:6F:28:AD:D4:CA"

# UUIDs (must match ESP32 code)
SERVICE_UUID = "dd3a359d-a0fb-49c2-9ba1-aae162aa2bdc"
COMMAND_CHAR_UUID = "b1d3e840-56fb-41d8-9791-bd8566220d03"
STATUS_CHAR_UUID = "7c6479dd-f6c6-4f68-8f1f-0cb38a06b1d3"



# Commands
BLE_CMD_OPEN = bytes([0x01])
BLE_CMD_CLOSE = bytes([0x00])

class BLELatchController:
    def __init__(self, device_name="ESP32-Latch"):
        self.device_name = device_name
        self.client = None
        self.address = None
        
    async def find_device(self):
        """Scan for the ESP32 latch device"""
        print(f"Scanning for '{self.device_name}'...")
        
        devices = await BleakScanner.discover(timeout=5.0)
        
        for device in devices:
            if device.name == self.device_name:
                self.address = device.address
                print(f"Found device: {device.name} ({device.address})")
                return True
        
        print(f"Device '{self.device_name}' not found")
        return False
    
    async def connect(self):
        """Connect to the BLE device"""
        if not self.address:
            if not await self.find_device():
                return False
        
        print(f"Connecting to {self.address}...")
        self.client = BleakClient(self.address)
        
        try:
            await self.client.connect()
            print("Connected successfully!")
            return True
        except Exception as e:
            print(f"Connection failed: {e}")
            return False
    
    async def disconnect(self):
        """Disconnect from the BLE device"""
        if self.client and self.client.is_connected:
            await self.client.disconnect()
            print("Disconnected")
    
    async def open_latch(self):
        """Send command to open the latch"""
        if not self.client or not self.client.is_connected:
            print("Not connected!")
            return False
        
        try:
            print("Sending OPEN command...")
            await self.client.write_gatt_char(COMMAND_CHAR_UUID, BLE_CMD_OPEN)
            print("✓ Latch opened")
            return True
        except Exception as e:
            print(f"Error opening latch: {e}")
            return False
    
    async def close_latch(self):
        """Send command to close the latch"""
        if not self.client or not self.client.is_connected:
            print("Not connected!")
            return False
        
        try:
            print("Sending CLOSE command...")
            await self.client.write_gatt_char(COMMAND_CHAR_UUID, BLE_CMD_CLOSE)
            print("✓ Latch closed")
            return True
        except Exception as e:
            print(f"Error closing latch: {e}")
            return False
    
    async def read_status(self):
        """Read the current latch status"""
        if not self.client or not self.client.is_connected:
            print("Not connected!")
            return None
        
        try:
            status_bytes = await self.client.read_gatt_char(STATUS_CHAR_UUID)
            status = status_bytes[0]
            status_str = "OPEN" if status == 0x01 else "CLOSED"
            print(f"Current status: {status_str} (0x{status:02X})")
            return status
        except Exception as e:
            print(f"Error reading status: {e}")
            return None
    
    def status_notification_handler(self, sender, data):
        """Handle status notifications"""
        status = data[0]
        status_str = "OPEN" if status == 0x01 else "CLOSED"
        print(f"[Notification] Latch status changed: {status_str}")
    
    async def subscribe_to_status(self):
        """Subscribe to status notifications"""
        if not self.client or not self.client.is_connected:
            print("Not connected!")
            return False
        
        try:
            await self.client.start_notify(STATUS_CHAR_UUID, self.status_notification_handler)
            print("Subscribed to status notifications")
            return True
        except Exception as e:
            print(f"Error subscribing to notifications: {e}")
            return False
    
    async def unsubscribe_from_status(self):
        """Unsubscribe from status notifications"""
        if not self.client or not self.client.is_connected:
            return
        
        try:
            await self.client.stop_notify(STATUS_CHAR_UUID)
            print("Unsubscribed from status notifications")
        except Exception as e:
            print(f"Error unsubscribing: {e}")


async def interactive_mode(controller):
    """Interactive mode for manual testing"""
    print("\n" + "="*50)
    print("Interactive Latch Controller")
    print("="*50)
    print("Commands:")
    print("  o - Open latch")
    print("  c - Close latch")
    print("  s - Read status")
    print("  n - Subscribe to notifications")
    print("  q - Quit")
    print("="*50 + "\n")
    
    # Subscribe to notifications by default
    await controller.subscribe_to_status()
    
    loop = asyncio.get_event_loop()
    
    while True:
        try:
            # Use run_in_executor to make input() non-blocking
            cmd = await loop.run_in_executor(None, input, "\nEnter command: ")
            cmd = cmd.strip().lower()
            
            if cmd == 'o':
                await controller.open_latch()
            elif cmd == 'c':
                await controller.close_latch()
            elif cmd == 's':
                await controller.read_status()
            elif cmd == 'n':
                await controller.subscribe_to_status()
            elif cmd == 'q':
                break
            else:
                print("Invalid command")
        except KeyboardInterrupt:
            print("\nInterrupted")
            break
    
    await controller.unsubscribe_from_status()


async def main():
    """Main function"""
    # Check command line arguments
    device_name = DEFAULT_BLE_DEVICE_NAME
    
    if len(sys.argv) > 1:
        device_name = sys.argv[1]
    
    controller = BLELatchController(device_name)
    
    try:
        # Connect to device
        if not await controller.connect():
            return
        
        await interactive_mode(controller)
        
    except Exception as e:
        print(f"Error: {e}")
    finally:
        # Clean disconnect
        await controller.disconnect()


if __name__ == "__main__":
    print("BLE Latch Controller - Python Test Script")
    print("Install dependencies: pip install bleak\n")
    
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nExiting...")