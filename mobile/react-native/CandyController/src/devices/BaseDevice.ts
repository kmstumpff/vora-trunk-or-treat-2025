import {
  BleError,
  Device,
  type Base64,
  type Characteristic,
  type UUID,
} from "react-native-ble-plx";

export default class BaseDevice {
  rawDevice: Device;
  private subscription: any;
  private hasBeenInitialized: boolean = false;

  constructor(rawDevice: Device) {
    this.rawDevice = rawDevice;
  }

  async initialize() {
    try {
      if (!this.hasBeenInitialized) {
        // Discover services and characteristics
        await this.rawDevice.discoverAllServicesAndCharacteristics();
        this.hasBeenInitialized = true;
      }
    } catch (error) {
      console.error("Initialization error:", error);
    }
  }

  getName(): string | null {
    return this.rawDevice.name;
  }

  async startStatusNotifications(
    serviceUUID: UUID,
    statusCharacteristic: UUID,
    callback?: (data: string) => void
  ) {
    if (this.subscription) {
      this.subscription.remove();
      this.subscription = null;
    }

    this.subscription = this.rawDevice.monitorCharacteristicForService(
      serviceUUID,
      statusCharacteristic,
      (error: BleError | null, characteristic: Characteristic | null) => {
        if (error) {
          console.error("Status notification error:", error);
          return;
        }
        // this.handleStatusUpdate(characteristic);

        if (characteristic?.value) {
          const data = this.decodeStatus(characteristic.value);
          callback?.(data);
        }
      }
    );
  }

  decodeStatus(base64Value: Base64): string {
    // Override in child class
    throw new Error("decodeStatus must be implemented");
  }

  async writeCommand(
    serviceUUID: UUID,
    statusCharacteristic: UUID,
    commandData: any
  ) {
    const base64Data = this.encodeCommand(commandData);
    await this.rawDevice.writeCharacteristicWithResponseForService(
      serviceUUID,
      statusCharacteristic,
      base64Data
    );
  }

  encodeCommand(commandData: any): Base64 {
    // Override in child class
    throw new Error("encodeCommand must be implemented");
  }
}
