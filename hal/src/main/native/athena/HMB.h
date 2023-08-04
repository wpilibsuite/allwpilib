// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#define HAL_HMB_TIMESTAMP_OFFSET 5

// #define HAL_HMB_AI0 0x0
// #define HAL_HMB_AIAVERAGED0 0x10
// #define HAL_HMB_ACCUMULATOR0 0x20
// #define HAL_HMB_ACCUMULATOR1 0x30
// #define HAL_HMB_DIO 0x40
// #define HAL_HMB_ANALOGTRIGGERS 0x50
// #define HAL_HMB_COUNTERS 0x60
// #define HAL_HMB_COUNTERTIMERS 0x70
// #define HAL_HMB_ENCODERS 0x80
// #define HAL_HMB_ENCODERTIMERS 0x90
// #define HAL_HMB_DUTYCYCLE 0xA0
// #define HAL_HMB_INTERRUPTS 0xB0
// #define HAL_HMB_PWM 0xC0
// #define HAL_HMB_PWM_MXP 0xD0
// #define HAL_HMB_RELAY_DO_AO 0xE0
// #define HAL_HMB_TIMESTAMP_LOWER 0xF0
// #define HAL_HMB_TIMESTAMP_UPPER 0xF1

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
    uint64_t Value;
    uint32_t Offset;
    uint32_t Reserved;
    uint32_t Reserved2[12];
  } Accumulator0;
  struct Accumulator1 {
    uint64_t Value;
    uint32_t Offset;
    uint32_t Reserved[13];
  } Accumulator1;
  struct DI {
    uint32_t Values;
    uint32_t FilteredValues;
    uint32_t Reserved[14];
  } DI;
  struct AnalogTriggers {
    uint32_t Reserved[16];
  } AnalogTriggers;
  struct Counters {
    uint32_t Reserved[16];
  } Counters;
  struct CounterTimers {
    uint32_t Reserved[16];
  } CounterTimers;
  struct Encoders {
    uint32_t Reserved[16];
  } Encoders;
  struct EncoderTimers {
    uint32_t Reserved[16];
  } EncoderTimers;
  struct DutyCycle {
    uint32_t Reserved[16];
  } DutyCycle;
  struct Interrupts {
    uint32_t Reserved[16];
  } Interrupts;
  struct PWM {
    uint32_t Reserved[32];
  } PWM;
  struct RelayDOAO {
    uint32_t Reserved[16];
  } RelayDOAO;
  struct Timestamp {
    uint32_t Lower;
    uint32_t Upper;
  } Timestamp;
};

extern "C" {

void HAL_InitializeHMB(int32_t* status);

volatile HAL_HMBData* HAL_GetHMBBuffer(void);
}  // extern "C"
