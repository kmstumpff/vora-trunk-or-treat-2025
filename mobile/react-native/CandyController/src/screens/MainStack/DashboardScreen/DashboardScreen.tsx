import { useLatch } from '@/contexts/LatchContext';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import React from 'react';
import { AppButton, AppText, CenteredScreenContainer } from '../../../components/atoms';
import type { MainStackParamList } from '../../../navigation/navigators';

type DashboardScreenProps = NativeStackScreenProps<MainStackParamList, 'DASHBOARD_SCREEN'>;

export function DashboardScreen({ navigation }: DashboardScreenProps) {
  const { device, isConnected, disconnect } = useLatch();

  return (
    <CenteredScreenContainer>
      {isConnected ? (
        <>
          <AppText style={{ marginBottom: 20, textAlign: 'center' }}>
            Connected to {device?.getName() ?? 'Unknown Device'}
          </AppText>
          <AppButton label="Disconnect" onPress={disconnect} />
          <AppButton label="View Details" onPress={() => navigation.navigate('DEVICE_DETAILS_SCREEN')} />
        </>
      ) : (
        <AppButton label="Connect" onPress={() => navigation.navigate('DEVICE_CONNECT_SCREEN')} />
      )}
    </CenteredScreenContainer>
  );
}
