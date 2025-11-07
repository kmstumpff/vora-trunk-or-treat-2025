import { LatchProvider } from "@/contexts/LatchContext";
import React from "react";
import { SafeAreaProvider } from "react-native-safe-area-context";
import Toast from "react-native-toast-message";
import { ThemeProvider } from "styled-components";
import ErrorBoundary from "./components/ErrorBoundary";
import { Navigation } from "./navigation";
import { commonTheme } from "./theme/theme";

export function App() {
  return (
    <ErrorBoundary>
      <SafeAreaProvider>
        <ThemeProvider theme={commonTheme}>
          <LatchProvider>
            <Navigation />
            <Toast />
          </LatchProvider>
        </ThemeProvider>
      </SafeAreaProvider>
    </ErrorBoundary>
  );
}
