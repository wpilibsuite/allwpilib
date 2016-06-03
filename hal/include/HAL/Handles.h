/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#define HAL_INVALID_HANDLE 0
#define HAL_HANDLE_INDEX_OUT_OF_RANGE -1
#define HAL_HANDLE_ALREADY_ALLOCATED -2
#define HAL_HANDLE_INVALID_TYPE -3
#define HAL_HANDLE_OUT_OF_HANDLES -4
#define HAL_HANDLE_NEGATIVE_INDEX -5

typedef int32_t HalHandle;
