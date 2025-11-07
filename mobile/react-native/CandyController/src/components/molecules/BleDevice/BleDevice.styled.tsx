import { TouchableOpacity, View } from 'react-native';
import styled, { css } from 'styled-components';

export const PressableContainer = styled(TouchableOpacity)`
  ${({ theme }) => css`
    border-color: ${theme.colors.primary};
    border-width: 1px;
    padding: 12px;
    border-radius: 12px;
    margin-top: 12px;
  `}
`;
export const StaticContainer = styled(View)`
  ${({ theme }) => css`
    border-color: ${theme.colors.primary};
    border-width: 1px;
    padding: 12px;
    border-radius: 12px;
    margin-top: 12px;
  `}
`;
