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
 * Method:    setFilterSelect
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DigitalGlitchFilterJNI_setFilterSelect
  (JNIEnv* env, jclass, jint id, jint filter_index)
{
  int32_t status = 0;

  HAL_SetFilterSelect(static_cast<HAL_DigitalHandle>(id), filter_index,
                      &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DigitalGlitchFilterJNI
 * Method:    getFilterSelect
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DigitalGlitchFilterJNI_getFilterSelect
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;

  jint result =
      HAL_GetFilterSelect(static_cast<HAL_DigitalHandle>(id), &status);
  CheckStatus(env, status);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_DigitalGlitchFilterJNI
 * Method:    setFilterPeriod
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DigitalGlitchFilterJNI_setFilterPeriod
  (JNIEnv* env, jclass, jint filter_index, jint fpga_cycles)
{
  int32_t status = 0;

  HAL_SetFilterPeriod(filter_index, fpga_cycles, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DigitalGlitchFilterJNI
 * Method:    getFilterPeriod
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DigitalGlitchFilterJNI_getFilterPeriod
  (JNIEnv* env, jclass, jint filter_index)
{
  int32_t status = 0;

  jint result = HAL_GetFilterPeriod(filter_index, &status);
  CheckStatus(env, status);
  return result;
}

}  // extern "C"
