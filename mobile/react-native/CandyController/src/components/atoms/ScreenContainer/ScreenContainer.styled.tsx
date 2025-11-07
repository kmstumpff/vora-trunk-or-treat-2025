import { SafeAreaView } from 'react-native-safe-area-context';
import styled, { css } from 'styled-components';

export const DefaultContainer = styled(SafeAreaView)`
  ${({ theme }) => css`
    flex: 1;
    padding: ${theme.sizes.defaultScreenPadding}px;
  `}
`;

export const CenteredContainer = styled(DefaultContainer)`
  justify-content: center;
`;
