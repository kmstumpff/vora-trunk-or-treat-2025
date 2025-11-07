import { createNativeStackNavigator } from '@react-navigation/native-stack';
import React from 'react';
import * as screenComponents from '../../screens';
import { useCommonScreenOptions } from '../components';

export type MainStackParamList = {
  DASHBOARD_SCREEN: undefined;
  DEVICE_CONNECT_SCREEN: undefined;
  DEVICE_DETAILS_SCREEN: undefined;
};

const MainStack = createNativeStackNavigator<MainStackParamList>();

export function MainStackComponent() {
  const commonScreenOptions = useCommonScreenOptions();

  return (
    <MainStack.Navigator screenOptions={commonScreenOptions}>
      <MainStack.Screen
        name="DASHBOARD_SCREEN"
        component={screenComponents.DashboardScreen}
        options={{
          headerTitle: 'Candy Controller',
        }}
      />
      <MainStack.Screen
        name="DEVICE_CONNECT_SCREEN"
        component={screenComponents.DeviceConnectScreen}
        options={{
          headerTitle: 'Connect',
          headerBackVisible: true,
        }}
      />
      <MainStack.Screen
        name="DEVICE_DETAILS_SCREEN"
        component={screenComponents.DeviceDetailsScreen}
        options={{
          headerTitle: 'Device Details',
          headerBackVisible: true,
        }}
      />
    </MainStack.Navigator>
  );
}
