/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_DutyCycleJNI.h"
#include "hal/DutyCycle.h"

using namespace hal;

extern "C" {
/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    initialize
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DutyCycleJNI_initialize
  (JNIEnv* env, jclass, jint digitalSourceHandle, jint analogTriggerType)
{
  int32_t status = 0;
  auto handle = HAL_InitializeDutyCycle(
      static_cast<HAL_Handle>(digitalSourceHandle),
      static_cast<HAL_AnalogTriggerType>(analogTriggerType), &status);
  CheckStatus(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DutyCycleJNI_free
  (JNIEnv*, jclass, jint handle)
{
  HAL_FreeDutyCycle(static_cast<HAL_DutyCycleHandle>(handle));
}

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getFrequency
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DutyCycleJNI_getFrequency
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFrequency(
      static_cast<HAL_DutyCycleHandle>(handle), &status);
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getOutput
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_DutyCycleJNI_getOutput
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto retVal =
      HAL_GetDutyCycleOutput(static_cast<HAL_DutyCycleHandle>(handle), &status);
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getOutputRaw
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DutyCycleJNI_getOutputRaw
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutputRaw(
      static_cast<HAL_DutyCycleHandle>(handle), &status);
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getOutputScaleFactor
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DutyCycleJNI_getOutputScaleFactor
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleOutputScaleFactor(
      static_cast<HAL_DutyCycleHandle>(handle), &status);
  CheckStatus(env, status);
  return retVal;
}

/*
 * Class:     edu_wpi_first_hal_DutyCycleJNI
 * Method:    getFPGAIndex
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DutyCycleJNI_getFPGAIndex
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto retVal = HAL_GetDutyCycleFPGAIndex(
      static_cast<HAL_DutyCycleHandle>(handle), &status);
  CheckStatus(env, status);
  return retVal;
}

}  // extern "C"
