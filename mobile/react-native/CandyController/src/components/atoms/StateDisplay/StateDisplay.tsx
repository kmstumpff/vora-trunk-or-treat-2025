import type { BLEScanState } from "@/types";
import React from "react";
import { AppText } from "../AppText/AppText";
import { Container, Header, Label } from "./StateDisplay.styled";

export type StateDisplayProps = {
  label?: string;
  state?: BLEScanState;
  value?: string;
};

const marks: Record<BLEScanState, string> = {
  DONE: "\u2705",
  ERROR: "\u274C",
  WAITING: "\u231B",
  IN_PROGRESS: "\u260E",
  CONNECTING: "\u26A1",
};

export function StateDisplay({ label, state, value }: StateDisplayProps) {
  return (
    <Container>
      <Header>
        <Label>{label}</Label>
        {!!state && <AppText>{marks[state]}</AppText>}
      </Header>
      {!!value && <AppText>{value}</AppText>}
    </Container>
  );
}
