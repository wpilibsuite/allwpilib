// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "HALUtil.h"
#include "edu_wpi_first_hal_LEDJNI.h"
#include "hal/LEDs.h"

static_assert(edu_wpi_first_hal_LEDJNI_RADIO_LED_STATE_OFF ==
              HAL_RadioLEDState::HAL_RadioLED_kOff);
static_assert(edu_wpi_first_hal_LEDJNI_RADIO_LED_STATE_GREEN ==
              HAL_RadioLEDState::HAL_RadioLED_kGreen);
static_assert(edu_wpi_first_hal_LEDJNI_RADIO_LED_STATE_RED ==
              HAL_RadioLEDState::HAL_RadioLED_kRed);
static_assert(edu_wpi_first_hal_LEDJNI_RADIO_LED_STATE_ORANGE ==
              HAL_RadioLEDState::HAL_RadioLED_kOrange);

using namespace hal;

extern "C" {
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
  HAL_SetRadioLEDState(static_cast<HAL_RadioLEDState>(state), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_LEDJNI
 * Method:    getRadioLEDState
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_LEDJNI_getRadioLEDState
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  auto retVal = HAL_GetRadioLEDState(&status);
  CheckStatus(env, status);
  return retVal;
}
}  // extern "C"
