import { BleDevice } from '@/components/molecules';
import { useLatch } from '@/contexts/LatchContext';
import { LatchStatus } from '@/types/LatchStatus';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import React, { useEffect } from 'react';
import { ScrollView } from 'react-native';
import { AppButton, AppText, CenteredScreenContainer } from '../../../components/atoms';
import type { MainStackParamList } from '../../../navigation/navigators';

type DeviceDetailsScreenProps = NativeStackScreenProps<MainStackParamList, 'DEVICE_DETAILS_SCREEN'>;

export function DeviceDetailsScreen(_props: DeviceDetailsScreenProps) {
  const { device } = useLatch();
  const [isLocked, setIsLocked] = React.useState<boolean>(false);

  const onStatusUpdate = (status: LatchStatus) => {
    setIsLocked(status === LatchStatus.LOCKED);
  };

  useEffect(() => {
    if (device) {
      const currentStatus = device.getStatus();
      setIsLocked(currentStatus === LatchStatus.LOCKED);
      device.onStatusUpdate(onStatusUpdate);
    }
    return () => {
      device?.onStatusUpdate(() => {});
    };
  }, [device]);

  const toggleDevice = async () => {
    device?.setLatchState(isLocked ? LatchStatus.UNLOCKED : LatchStatus.LOCKED);
  };

  return (
    <CenteredScreenContainer>
      <ScrollView>
        <AppText style={{ marginBottom: 20, textAlign: 'center' }}>
          Device is currently: {isLocked ? 'Locked' : 'Unlocked'}
        </AppText>
        <AppButton label="Toggle" onPress={toggleDevice} />
        {/* <AppText>{device?.getDetails()}</AppText> */}
        {device?.rawDevice && <BleDevice device={device?.rawDevice} />}
      </ScrollView>
    </CenteredScreenContainer>
  );
}
