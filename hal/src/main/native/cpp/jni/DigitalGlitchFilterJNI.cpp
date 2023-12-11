// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_DigitalGlitchFilterJNI.h"
#include "hal/DIO.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_DigitalGlitchFilterJNI
 * Method:    setFilterFrequency
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DigitalGlitchFilterJNI_setFilterFrequency
  (JNIEnv* env, jclass, jint dioPortHandle, jdouble frequencyHertz)
{
  int32_t status = 0;
  HAL_SetFilterFrequency(dioPortHandle, frequencyHertz, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DigitalGlitchFilterJNI
 * Method:    getFilterFrequency
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_DigitalGlitchFilterJNI_getFilterFrequency
  (JNIEnv* env, jclass, jint dioPortHandle)
{
  int32_t status = 0;
  double result = HAL_GetFilterFrequency(dioPortHandle, &status);
  CheckStatus(env, status);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_DigitalGlitchFilterJNI
 * Method:    disableFilter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DigitalGlitchFilterJNI_disableFilter
  (JNIEnv* env, jclass, jint dioPortHandle)
{
  int32_t status = 0;
  HAL_DisableFilter(dioPortHandle, &status);
  CheckStatus(env, status);
}

}  // extern "C"
