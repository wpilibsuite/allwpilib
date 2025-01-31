// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_AnalogJNI.h"
#include "hal/AnalogInput.h"
#include "hal/AnalogTrigger.h"
#include "hal/Ports.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    initializeAnalogInputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_initializeAnalogInputPort
  (JNIEnv* env, jclass, jint channel)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto analog = HAL_InitializeAnalogInputPort(channel, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return (jint)analog;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    freeAnalogInputPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_freeAnalogInputPort
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreeAnalogInputPort((HAL_AnalogInputHandle)id);
  }
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    checkAnalogModule
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_checkAnalogModule
  (JNIEnv*, jclass, jbyte value)
{
  jboolean returnValue = HAL_CheckAnalogModule(value);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    checkAnalogInputChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_checkAnalogInputChannel
  (JNIEnv*, jclass, jint value)
{
  jboolean returnValue = HAL_CheckAnalogInputChannel(value);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogInputSimDevice
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogInputSimDevice
  (JNIEnv* env, jclass, jint handle, jint device)
{
  HAL_SetAnalogInputSimDevice((HAL_AnalogInputHandle)handle,
                              (HAL_SimDeviceHandle)device);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogSampleRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogSampleRate
  (JNIEnv* env, jclass, jdouble value)
{
  int32_t status = 0;
  HAL_SetAnalogSampleRate(value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogSampleRate
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogSampleRate
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double returnValue = HAL_GetAnalogSampleRate(&status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogAverageBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogAverageBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetAnalogAverageBits((HAL_AnalogInputHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogAverageBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogAverageBits
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageBits((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogOversampleBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogOversampleBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetAnalogOversampleBits((HAL_AnalogInputHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogOversampleBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogOversampleBits
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogOversampleBits((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogValue
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogValue
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jshort returnValue = HAL_GetAnalogValue((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogAverageValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogAverageValue
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageValue((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogVoltsToValue
 * Signature: (ID)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogVoltsToValue
  (JNIEnv* env, jclass, jint id, jdouble voltageValue)
{
  int32_t status = 0;
  jint returnValue = HAL_GetAnalogVoltsToValue((HAL_AnalogInputHandle)id,
                                               voltageValue, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogValueToVolts
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogValueToVolts
  (JNIEnv* env, jclass, jint id, jint rawValue)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogValueToVolts((HAL_AnalogInputHandle)id, rawValue, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogVoltage
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogVoltage((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogAverageVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogAverageVoltage
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogAverageVoltage((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogLSBWeight
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogLSBWeight
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogLSBWeight((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogOffset
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogOffset
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogOffset((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    initializeAnalogTrigger
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_initializeAnalogTrigger
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_AnalogTriggerHandle analogTrigger =
      HAL_InitializeAnalogTrigger((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return (jint)analogTrigger;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    initializeAnalogTriggerDutyCycle
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_initializeAnalogTriggerDutyCycle
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_AnalogTriggerHandle analogTrigger =
      HAL_InitializeAnalogTriggerDutyCycle((HAL_DutyCycleHandle)id, &status);
  CheckStatus(env, status);
  return (jint)analogTrigger;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    cleanAnalogTrigger
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_cleanAnalogTrigger
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_CleanAnalogTrigger((HAL_AnalogTriggerHandle)id);
  }
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsRaw
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerLimitsRaw
  (JNIEnv* env, jclass, jint id, jint lower, jint upper)
{
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsRaw((HAL_AnalogTriggerHandle)id, lower, upper,
                                &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsDutyCycle
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerLimitsDutyCycle
  (JNIEnv* env, jclass, jint id, jdouble lower, jdouble upper)
{
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsDutyCycle((HAL_AnalogTriggerHandle)id, lower, upper,
                                      &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsVoltage
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerLimitsVoltage
  (JNIEnv* env, jclass, jint id, jdouble lower, jdouble upper)
{
  int32_t status = 0;
  HAL_SetAnalogTriggerLimitsVoltage((HAL_AnalogTriggerHandle)id, lower, upper,
                                    &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerAveraged
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerAveraged
  (JNIEnv* env, jclass, jint id, jboolean averaged)
{
  int32_t status = 0;
  HAL_SetAnalogTriggerAveraged((HAL_AnalogTriggerHandle)id, averaged, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    setAnalogTriggerFiltered
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_AnalogJNI_setAnalogTriggerFiltered
  (JNIEnv* env, jclass, jint id, jboolean filtered)
{
  int32_t status = 0;
  HAL_SetAnalogTriggerFiltered((HAL_AnalogTriggerHandle)id, filtered, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogTriggerInWindow
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogTriggerInWindow
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean val =
      HAL_GetAnalogTriggerInWindow((HAL_AnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogTriggerTriggerState
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogTriggerTriggerState
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean val =
      HAL_GetAnalogTriggerTriggerState((HAL_AnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogTriggerOutput
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogTriggerOutput
  (JNIEnv* env, jclass, jint id, jint type)
{
  int32_t status = 0;
  jboolean val = HAL_GetAnalogTriggerOutput(
      (HAL_AnalogTriggerHandle)id, (HAL_AnalogTriggerType)type, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_AnalogJNI
 * Method:    getAnalogTriggerFPGAIndex
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_AnalogJNI_getAnalogTriggerFPGAIndex
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  auto val =
      HAL_GetAnalogTriggerFPGAIndex((HAL_AnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

}  // extern "C"
