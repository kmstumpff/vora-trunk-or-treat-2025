// Example ErrorBoundary.tsx
import React from "react";
import { Text } from "react-native";

export default class ErrorBoundary extends React.Component<
  React.PropsWithChildren<{}>
> {
  state = { hasError: false, error: null };
  static getDerivedStateFromError(error: any) {
    return { hasError: true, error };
  }
  componentDidCatch(error: any, info: any) {
    console.error("ErrorBoundary caught:", error, info);
  }
  render() {
    if (this.state.hasError) {
      return <Text>Error: {String(this.state.error)}</Text>;
    }
    return this.props.children;
  }
}
