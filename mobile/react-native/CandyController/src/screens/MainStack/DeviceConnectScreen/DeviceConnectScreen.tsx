import { AppButton, DefaultScreenContainer, StateDisplay } from '@/components/atoms';
import { BleDevice } from '@/components/molecules';
import { useLatch } from '@/contexts/LatchContext';
import type { MainStackParamList } from '@/navigation/navigators';
import type { BLEScanState } from '@/types';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import cloneDeep from 'lodash/cloneDeep';
import React, { useState } from 'react';
import { FlatList } from 'react-native';
import { Device } from 'react-native-ble-plx';
import Toast from 'react-native-toast-message';

type DeviceConnectScreenProps = NativeStackScreenProps<MainStackParamList, 'DEVICE_CONNECT_SCREEN'>;
type DeviceExtendedByUpdateTime = Device & { updateTimestamp: number };

const MIN_TIME_BEFORE_UPDATE_IN_MILLISECONDS = 5000;
const SERVICE_UUID = 'dd3a359d-a0fb-49c2-9ba1-aae162aa2bdc';

export function DeviceConnectScreen({ navigation }: DeviceConnectScreenProps) {
  const { bleManager } = useLatch();
  const [scanDevicesState, setScanDevicesState] = useState<BLEScanState>('WAITING');
  const [foundDevices, setFoundDevices] = useState<Array<any>>([]);

  const isCompatableDevice = (device: Device) => {
    return device.serviceUUIDs?.find((uuid) => uuid === SERVICE_UUID) !== undefined;
  };

  const addFoundDevice = async (device: Device) => {
    if (isCompatableDevice(device)) {
      setFoundDevices((prevState) => {
        if (!isFoundDeviceUpdateNecessary(prevState, device)) {
          return prevState;
        }
        // deep clone
        const nextState = cloneDeep(prevState);
        const extendedDevice: DeviceExtendedByUpdateTime = {
          ...device,
          updateTimestamp: Date.now() + MIN_TIME_BEFORE_UPDATE_IN_MILLISECONDS,
        } as DeviceExtendedByUpdateTime;

        const indexToReplace = nextState.findIndex((currentDevice) => currentDevice.id === device.id);
        if (indexToReplace === -1) {
          return nextState.concat(extendedDevice);
        }
        nextState[indexToReplace] = extendedDevice;
        return nextState;
      });
    }
  };

  const isFoundDeviceUpdateNecessary = (currentDevices: DeviceExtendedByUpdateTime[], updatedDevice: Device) => {
    const currentDevice = currentDevices.find(({ id }) => updatedDevice.id === id);
    if (!currentDevice) {
      return true;
    }
    return currentDevice.updateTimestamp < Date.now();
  };

  const startScan = async () => {
    setFoundDevices([]);
    setScanDevicesState('IN_PROGRESS');
    bleManager.initializeBLE().then(() => bleManager.scanDevices(addFoundDevice, [SERVICE_UUID], true));
  };

  const onConnectSuccess = () => {
    setScanDevicesState('DONE');
    Toast.show({
      type: 'success',
      text1: 'Connected successfully',
    });
    navigation.navigate('DASHBOARD_SCREEN');
  };

  const onConnectFail = () => {
    setScanDevicesState('ERROR');
    Toast.show({
      type: 'error',
      text1: 'Connection failed',
    });
  };

  const deviceRender = (device: Device) => (
    <BleDevice
      onPress={(pickedDevice) => {
        setScanDevicesState('CONNECTING');
        bleManager.connectToDevice(pickedDevice.id).then(onConnectSuccess).catch(onConnectFail);
      }}
      key={device.id}
      device={device}
    />
  );

  return (
    <DefaultScreenContainer>
      <StateDisplay label="Scan state" state={scanDevicesState} value={scanDevicesState} />
      {scanDevicesState === 'IN_PROGRESS' && (
        <AppButton
          label="Cancel scan"
          onPress={() => {
            bleManager.cancelScan();
            setScanDevicesState('DONE');
          }}
        />
      )}
      {scanDevicesState !== 'IN_PROGRESS' && <AppButton label="Start scan" onPress={startScan} />}
      <FlatList
        style={{ flex: 1 }}
        data={foundDevices}
        renderItem={({ item }) => deviceRender(item)}
        keyExtractor={(device) => device.id}
      />
    </DefaultScreenContainer>
  );
}
