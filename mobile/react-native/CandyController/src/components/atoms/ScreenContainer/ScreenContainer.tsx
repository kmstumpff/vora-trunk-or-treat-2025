import React from 'react';
import { CenteredContainer, DefaultContainer } from './ScreenContainer.styled';

export type DefaultScreenContainerProps = {
  children: React.ReactNode;
};

export function DefaultScreenContainer({ children }: DefaultScreenContainerProps) {
  return <DefaultContainer>{children}</DefaultContainer>;
}

export function CenteredScreenContainer({ children }: DefaultScreenContainerProps) {
  return <CenteredContainer>{children}</CenteredContainer>;
}
