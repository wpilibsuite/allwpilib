// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

struct HAL_HMBData {
  struct AnalogInputs {
    uint32_t Values[8];
    uint32_t Reserved[8];
  } AnalogInputs;
  struct AveragedAnalogInputs {
    uint32_t Values[8];
    uint32_t Reserved[8];
  } AveragedAnalogInputs;
  struct Accumulator0 {
    uint32_t Count;
    uint32_t Value0;
    uint32_t Value1;
    uint32_t Reserved[13];
  } Accumulator0;
  struct Accumulator1 {
    uint32_t Count;
    uint32_t Value0;
    uint32_t Value1;
    uint32_t Reserved[13];
  } Accumulator1;
  struct DI {
    uint32_t Values;
    uint32_t FilteredValues;
    uint32_t Reserved[14];
  } DI;
  struct AnalogTriggers {
    struct Trigger {
      uint8_t InHysteresis0 : 1;
      uint8_t OverLimit0 : 1;
      uint8_t Rising0 : 1;
      uint8_t Falling0 : 1;
      uint8_t InHysteresis1 : 1;
      uint8_t OverLimit1 : 1;
      uint8_t Rising1 : 1;
      uint8_t Falling1 : 1;
    } Trigger[4];
    uint32_t Reserved[15];
  } AnalogTriggers;
  struct Counters {
    struct Counter {
      uint32_t Direction : 1;
      int32_t Value : 31;
    } Counter[8];
    uint32_t Reserved[8];
  } Counters;
  struct CounterTimers {
    struct Timer {
      uint32_t Period : 23;
      int32_t Count : 8;
      uint32_t Stalled : 1;
    } Timer[8];
    uint32_t Reserved[8];
  } CounterTimers;
  struct Encoders {
    struct Encoder {
      uint32_t Direction : 1;
      int32_t Value : 31;
    } Encoder[8];
    uint32_t Reserved[8];
  } Encoders;
  struct EncoderTimers {
    struct Timer {
      uint32_t Period : 23;
      int32_t Count : 8;
      uint32_t Stalled : 1;
    } Timer[8];
    uint32_t Reserved[8];
  } EncoderTimers;
  struct DutyCycle {
    uint32_t Output[8];
    uint32_t Reserved[8];
  } DutyCycle;
  struct Interrupts {
    struct Interrupt {
      uint32_t FallingTimestamp;
      uint32_t RisingTimestamp;
    } Interrupt[8];
  } Interrupts;
  struct PWM {
    uint32_t Headers[10];
    uint32_t Reserved[6];
    uint32_t MXP[10];
    uint32_t Reserved2[6];
  } PWM;
  struct RelayDOAO {
    uint32_t Relays;
    uint32_t Reserved;
    uint32_t AO[2];
    uint32_t Reserved2[12];
  } RelayDOAO;
  struct Timestamp {
    uint32_t Lower;
    uint32_t Upper;
    uint32_t Reserved[14];
  } Timestamp;
};

extern "C" {

void HAL_InitializeHMB(int32_t* status);

const volatile HAL_HMBData* HAL_GetHMBBuffer(void);
}  // extern "C"
