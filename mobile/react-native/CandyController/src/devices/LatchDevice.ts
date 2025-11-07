import { LatchStatus } from "@/types/LatchStatus";
import base64 from "react-native-base64";
import { Device, type Base64 } from "react-native-ble-plx";

import BaseDevice from "./BaseDevice";

export default class LatchDevice extends BaseDevice {
  statusListeners: Set<(status: LatchStatus) => void>;
  private LOCKED_CMD = 0x00;
  private UNLOCKED_CMD = 0x01;
  private SERVICE_UUID = "dd3a359d-a0fb-49c2-9ba1-aae162aa2bdc";
  private COMMAND_CHAR_UUID = "b1d3e840-56fb-41d8-9791-bd8566220d03";
  private STATUS_CHAR_UUID = "7c6479dd-f6c6-4f68-8f1f-0cb38a06b1d3";

  private status: LatchStatus | null = null;

  constructor(device: Device | BaseDevice) {
    if (device instanceof BaseDevice) {
      super(device.rawDevice);
    } else {
      super(device);
    }
    this.statusListeners = new Set();
    this.onStatusChange = this.onStatusChange.bind(this);
  }

  async initialize(): Promise<void> {
    await super.initialize();
    await this.subscribeToStatusChanges();
  }

  getDetails(): string | null {
    return JSON.stringify(this.rawDevice, null, 4);
  }

  getStatus(): LatchStatus | null {
    return this.status;
  }

  onStatusChange() {
    console.log("Latch status changed");
  }

  async subscribeToStatusChanges() {
    await this.startStatusNotifications(
      this.SERVICE_UUID,
      this.STATUS_CHAR_UUID,
      (data: string) => {
        const status = LatchStatus[data as keyof typeof LatchStatus];
        this.status = status;
        this.notifyStatusListeners(status);
      }
    );
  }

  decodeStatus(base64Value: Base64): string {
    const decoded = base64.decode(base64Value || "");
    const statusByte = decoded.charCodeAt(0);
    return statusByte === this.UNLOCKED_CMD
      ? LatchStatus.UNLOCKED
      : LatchStatus.LOCKED;
  }

  encodeCommand(command: string): Base64 {
    const commandByte =
      command === LatchStatus.UNLOCKED ? this.UNLOCKED_CMD : this.LOCKED_CMD;
    const commandBase64 = base64.encode(String.fromCharCode(commandByte));
    return commandBase64;
  }

  // Device-specific high-level methods
  async setLatchState(state: LatchStatus): Promise<void> {
    this.writeCommand(
      this.SERVICE_UUID,
      this.COMMAND_CHAR_UUID,
      state.valueOf()
    );
  }

  onStatusUpdate(callback: (status: LatchStatus) => void): () => void {
    this.statusListeners.add(callback);
    return () => this.statusListeners.delete(callback);
  }

  notifyStatusListeners(status: LatchStatus) {
    this.statusListeners.forEach((cb) => cb(status));
  }

  cleanup() {
    this.statusListeners.clear();
  }
}
