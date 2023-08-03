// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#define HAL_HMB_TIMESTAMP_OFFSET 5
#define HAL_HMB_TIMESTAMP_LOWER 0xF0
#define HAL_HMB_TIMESTAMP_UPPER 0xF1
#define HAL_HMB_DIO 0x40
#define HAL_HMB_DIO_FILTERED 0x41

extern "C" {

void HAL_InitializeHMB(int32_t* status);

volatile uint32_t* HAL_GetHMBBuffer(void);
}
