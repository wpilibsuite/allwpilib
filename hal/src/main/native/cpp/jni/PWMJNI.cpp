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
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto pwm = HAL_InitializePWMPort((HAL_PortHandle)id, stack.c_str(), &status);
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
 * Method:    setPWMConfigMicroseconds
 * Signature: (IIIIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMConfigMicroseconds
  (JNIEnv* env, jclass, jint id, jint maxPwm, jint deadbandMaxPwm,
   jint centerPwm, jint deadbandMinPwm, jint minPwm)
{
  int32_t status = 0;
  HAL_SetPWMConfigMicroseconds((HAL_DigitalHandle)id, maxPwm, deadbandMaxPwm,
                               centerPwm, deadbandMinPwm, minPwm, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMConfigMicroseconds
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMConfigMicroseconds
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  int32_t maxPwm = 0;
  int32_t deadbandMaxPwm = 0;
  int32_t centerPwm = 0;
  int32_t deadbandMinPwm = 0;
  int32_t minPwm = 0;
  HAL_GetPWMConfigMicroseconds((HAL_DigitalHandle)id, &maxPwm, &deadbandMaxPwm,
                               &centerPwm, &deadbandMinPwm, &minPwm, &status);
  CheckStatus(env, status);
  return CreatePWMConfigDataResult(env, maxPwm, deadbandMaxPwm, centerPwm,
                                   deadbandMinPwm, minPwm);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMEliminateDeadband
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMEliminateDeadband
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  int32_t status = 0;
  HAL_SetPWMEliminateDeadband((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMEliminateDeadband
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMEliminateDeadband
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  auto val = HAL_GetPWMEliminateDeadband((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return (jboolean)val;
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
 * Method:    setPWMSpeed
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMSpeed
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_SetPWMSpeed((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMPosition
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMPosition
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_SetPWMPosition((HAL_DigitalHandle)id, value, &status);
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
 * Method:    getPWMSpeed
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMSpeed
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue = HAL_GetPWMSpeed((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMPosition
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMPosition
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue = HAL_GetPWMPosition((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMDisabled
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMDisabled
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_SetPWMDisabled((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    latchPWMZero
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_latchPWMZero
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_LatchPWMZero((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setAlwaysHighMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setAlwaysHighMode
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_SetPWMAlwaysHighMode((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMPeriodScale
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMPeriodScale
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetPWMPeriodScale((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

}  // extern "C"
