// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#define HAL_HMB_TIMESTAMP_OFFSET 5

#define HAL_HMB_AI0 0x0
#define HAL_HMB_AIAVERAGED0 0x10
#define HAL_HMB_ACCUMULATOR0 0x20
#define HAL_HMB_ACCUMULATOR1 0x30
#define HAL_HMB_DIO 0x40
#define HAL_HMB_ANALOGTRIGGERS 0x50
#define HAL_HMB_COUNTERS 0x60
#define HAL_HMB_COUNTERTIMERS 0x70
#define HAL_HMB_ENCODERS 0x80
#define HAL_HMB_ENCODERTIMERS 0x90
#define HAL_HMB_DUTYCYCLE 0xA0
#define HAL_HMB_INTERRUPTS 0xB0
#define HAL_HMB_PWM 0xC0
#define HAL_HMB_PWM_MXP 0xD0
#define HAL_HMB_RELAY_DO_AO 0xE0
#define HAL_HMB_TIMESTAMP_LOWER 0xF0
#define HAL_HMB_TIMESTAMP_UPPER 0xF1

extern "C" {

void HAL_InitializeHMB(int32_t* status);

volatile uint32_t* HAL_GetHMBBuffer(void);
}
