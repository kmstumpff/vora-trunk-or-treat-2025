import React, {
  createContext,
  useContext,
  useEffect,
  useRef,
  useState,
} from "react";

import BaseDevice from "@/devices/BaseDevice";
import LatchDevice from "@/devices/LatchDevice";
import { BLEManager } from "@/services";

export interface LatchProviderProps {
  children: React.ReactNode;
}

interface LatchContextType {
  device: LatchDevice | null;
  isConnected: boolean;
  disconnect: () => Promise<void>;
  bleManager: BLEManager;
}

const LatchContext = createContext<LatchContextType | null>(null);

export function LatchProvider({ children }: LatchProviderProps) {
  const bleManager = useRef(new BLEManager()).current;
  const [device, setDevice] = useState<LatchDevice | null>(null);
  const [isConnected, setIsConnected] = useState(false);

  const onDeviceChange = (newDevice: BaseDevice | null) => {
    if (newDevice) {
      const latchDevice = new LatchDevice(newDevice);
      latchDevice.initialize().catch(() => {});
      setDevice(latchDevice);
      return;
    } else if (device) {
      device.cleanup();
    }
    setDevice(null);
  };

  useEffect(() => {
    const unsubscribe = bleManager.onDeviceChange(onDeviceChange);
    return () => {
      unsubscribe();
      if (bleManager.getDevice()) {
        bleManager.disconnectDevice().catch(() => {});
      }
    };
  }, []);

  useEffect(() => {
    setIsConnected(device !== null);
  }, [device]);

  const disconnect = async () => {
    try {
      if (device) {
        device.cleanup();
        await bleManager.disconnectDevice();
        setDevice(null);
      }
    } catch (error) {
      console.error("Disconnect error:", error);
    }
  };

  return (
    <>
      <LatchContext.Provider
        value={{
          device,
          isConnected,
          disconnect,
          bleManager,
        }}
      >
        {children}
      </LatchContext.Provider>
    </>
  );
}

export function useLatch() {
  const context = useContext(LatchContext);
  if (!context) {
    throw new Error("useLatch must be used within LatchProvider");
  }
  return context;
}
