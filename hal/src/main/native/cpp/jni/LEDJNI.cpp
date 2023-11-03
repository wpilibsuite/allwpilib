// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/core.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_LEDJNI.h"
#include "hal/LEDs.h"

using namespace hal;
/*
 * Class:     edu_wpi_first_hal_LEDJNI
 * Method:    setRadioLEDState
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_LEDJNI_setRadioLEDState
  (JNIEnv* env, jclass, jint state)
{
  int32_t status = 0;
  HAL_SetRadioLEDState((HAL_RadioLEDState)state, &status);
  CheckStatus(env, status);
}
