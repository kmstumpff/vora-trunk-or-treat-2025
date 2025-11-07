import React from 'react';
import { Device } from 'react-native-ble-plx';
import { PressableContainer, StaticContainer } from './BleDevice.styled';
import { DeviceProperty } from './DeviceProperty/DeviceProperty';

export type BleDeviceProps = {
  onPress?: (device: Device) => void;
  device: Device;
};

export function BleDevice({ device, onPress }: BleDeviceProps) {
  const isConnectableInfoValueIsUnavailable = typeof device.isConnectable !== 'boolean';
  const isConnectableValue = device.isConnectable ? 'true' : 'false';
  const parsedIsConnectable = isConnectableInfoValueIsUnavailable ? '-' : isConnectableValue;

  const getDetails = () => {
    return (
      <>
        <DeviceProperty name="name" value={device.name} />
        <DeviceProperty name="localName" value={device.localName} />
        <DeviceProperty name="id" value={device.id} />
        <DeviceProperty name="manufacturerData" value={device.manufacturerData} />
        <DeviceProperty name="rawScanRecord" value={device.rawScanRecord} />
        <DeviceProperty name="isConnectable" value={parsedIsConnectable} />
        <DeviceProperty name="mtu" value={device.mtu.toString()} />
        <DeviceProperty name="rssi" value={device.rssi} />
      </>
    );
  };

  if (!onPress) {
    return <StaticContainer>{getDetails()}</StaticContainer>;
  }

  return <PressableContainer onPress={() => onPress(device)}>{getDetails()}</PressableContainer>;
}
