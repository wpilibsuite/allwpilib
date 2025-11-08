// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_AnalogJNI.h"
#include "wpi/hal/AnalogInput.h"
#include "wpi/hal/Ports.h"
#include "wpi/hal/handles/HandlesInternal.h"
#include "wpi/util/jni_util.hpp"

using namespace hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    initializeAnalogInputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_initializeAnalogInputPort
  (JNIEnv* env, jclass, jint channel)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto analog = HAL_InitializeAnalogInputPort(channel, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return (jint)analog;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    freeAnalogInputPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_freeAnalogInputPort
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreeAnalogInputPort((HAL_AnalogInputHandle)id);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    checkAnalogModule
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_checkAnalogModule
  (JNIEnv*, jclass, jbyte value)
{
  jboolean returnValue = HAL_CheckAnalogModule(value);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    checkAnalogInputChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_checkAnalogInputChannel
  (JNIEnv*, jclass, jint value)
{
  jboolean returnValue = HAL_CheckAnalogInputChannel(value);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    setAnalogInputSimDevice
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_setAnalogInputSimDevice
  (JNIEnv* env, jclass, jint handle, jint device)
{
  HAL_SetAnalogInputSimDevice((HAL_AnalogInputHandle)handle,
                              (HAL_SimDeviceHandle)device);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    setAnalogSampleRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_setAnalogSampleRate
  (JNIEnv* env, jclass, jdouble value)
{
  int32_t status = 0;
  HAL_SetAnalogSampleRate(value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogSampleRate
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogSampleRate
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double returnValue = HAL_GetAnalogSampleRate(&status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    setAnalogAverageBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_setAnalogAverageBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetAnalogAverageBits((HAL_AnalogInputHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogAverageBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogAverageBits
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageBits((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    setAnalogOversampleBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_setAnalogOversampleBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetAnalogOversampleBits((HAL_AnalogInputHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogOversampleBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogOversampleBits
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogOversampleBits((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogValue
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogValue
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jshort returnValue = HAL_GetAnalogValue((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogAverageValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogAverageValue
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageValue((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogVoltsToValue
 * Signature: (ID)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogVoltsToValue
  (JNIEnv* env, jclass, jint id, jdouble voltageValue)
{
  int32_t status = 0;
  jint returnValue = HAL_GetAnalogVoltsToValue((HAL_AnalogInputHandle)id,
                                               voltageValue, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogValueToVolts
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogValueToVolts
  (JNIEnv* env, jclass, jint id, jint rawValue)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogValueToVolts((HAL_AnalogInputHandle)id, rawValue, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogVoltage
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogVoltage((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogAverageVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogAverageVoltage
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogAverageVoltage((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogLSBWeight
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogLSBWeight
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogLSBWeight((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogJNI
 * Method:    getAnalogOffset
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogJNI_getAnalogOffset
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogOffset((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
