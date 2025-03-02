// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_PWMJNI.h"
#include "hal/DIO.h"
#include "hal/PWM.h"
#include "hal/Ports.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    initializePWMPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PWMJNI_initializePWMPort
  (JNIEnv* env, jclass, jint channel)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto pwm = HAL_InitializePWMPort(channel, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return (jint)pwm;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    checkPWMChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PWMJNI_checkPWMChannel
  (JNIEnv* env, jclass, jint channel)
{
  return HAL_CheckPWMChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    freePWMPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_freePWMPort
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreePWMPort((HAL_DigitalHandle)id);
  }
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPulseTimeMicroseconds
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPulseTimeMicroseconds
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetPWMPulseTimeMicroseconds((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPulseTimeMicroseconds
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPulseTimeMicroseconds
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  int32_t returnValue =
      HAL_GetPWMPulseTimeMicroseconds((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMOutputPeriod
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMOutputPeriod
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetPWMOutputPeriod((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

}  // extern "C"
