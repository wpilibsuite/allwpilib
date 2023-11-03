// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include "hal/Types.h"

HAL_ENUM(HAL_RadioLEDState){HAL_RadioLED_kOFF = 0, HAL_RadioLED_kGreen = 1,
                            HAL_RadioLED_kRed = 2, HAL_RadioLED_kOrange = 3};

void HAL_SetRadioLEDState(HAL_RadioLEDState state, int32_t* status);
