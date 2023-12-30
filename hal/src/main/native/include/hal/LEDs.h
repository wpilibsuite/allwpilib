// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include "hal/Types.h"

HAL_ENUM(HAL_RadioLEDState) {
  HAL_RadioLED_kOff = 0,
  HAL_RadioLED_kGreen = 1,
  HAL_RadioLED_kRed = 2,
  HAL_RadioLED_kOrange = 3
};

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Set the state of the "Radio" LED.
 * @param state The state to set the LED to.
 * @param[out] status the error code, or 0 for success
 */
void HAL_SetRadioLEDState(HAL_RadioLEDState state, int32_t* status);

/**
 * Get the state of the "Radio" LED.
 *
 * @param[out] status the error code, or 0 for success
 * @return The state of the LED.
 */
HAL_RadioLEDState HAL_GetRadioLEDState(int32_t* status);
#ifdef __cplusplus
}  // extern "C"
#endif
