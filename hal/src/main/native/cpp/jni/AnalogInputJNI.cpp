// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.hpp"
#include "org_wpilib_hardware_hal_AnalogInputJNI.h"
#include "wpi/hal/AnalogInput.h"
#include "wpi/hal/Ports.h"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    initializeAnalogInputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_initializeAnalogInputPort
  (JNIEnv* env, jclass, jint channel)
{
  int32_t status = 0;
  auto stack = wpi::util::java::GetJavaStackTrace(env, "org.wpilib");
  auto analog = HAL_InitializeAnalogInputPort(channel, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return (jint)analog;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    freeAnalogInputPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_freeAnalogInputPort
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_INVALID_HANDLE) {
    HAL_FreeAnalogInputPort((HAL_AnalogInputHandle)id);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    checkAnalogModule
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_checkAnalogModule
  (JNIEnv*, jclass, jbyte value)
{
  jboolean returnValue = HAL_CheckAnalogModule(value);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    checkAnalogInputChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_checkAnalogInputChannel
  (JNIEnv*, jclass, jint value)
{
  jboolean returnValue = HAL_CheckAnalogInputChannel(value);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    setAnalogInputSimDevice
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_setAnalogInputSimDevice
  (JNIEnv* env, jclass, jint handle, jint device)
{
  HAL_SetAnalogInputSimDevice((HAL_AnalogInputHandle)handle,
                              (HAL_SimDeviceHandle)device);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    setAnalogSampleRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_setAnalogSampleRate
  (JNIEnv* env, jclass, jdouble value)
{
  int32_t status = 0;
  HAL_SetAnalogSampleRate(value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogSampleRate
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogSampleRate
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  double returnValue = HAL_GetAnalogSampleRate(&status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    setAnalogAverageBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_setAnalogAverageBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetAnalogAverageBits((HAL_AnalogInputHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogAverageBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogAverageBits
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageBits((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    setAnalogOversampleBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_setAnalogOversampleBits
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetAnalogOversampleBits((HAL_AnalogInputHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogOversampleBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogOversampleBits
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogOversampleBits((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogValue
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogValue
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jshort returnValue = HAL_GetAnalogValue((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogAverageValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogAverageValue
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetAnalogAverageValue((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogVoltsToValue
 * Signature: (ID)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogVoltsToValue
  (JNIEnv* env, jclass, jint id, jdouble voltageValue)
{
  int32_t status = 0;
  jint returnValue = HAL_GetAnalogVoltsToValue((HAL_AnalogInputHandle)id,
                                               voltageValue, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogValueToVolts
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogValueToVolts
  (JNIEnv* env, jclass, jint id, jint rawValue)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogValueToVolts((HAL_AnalogInputHandle)id, rawValue, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogVoltage
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogVoltage((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogAverageVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogAverageVoltage
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue =
      HAL_GetAnalogAverageVoltage((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogLSBWeight
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogLSBWeight
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogLSBWeight((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_AnalogInputJNI
 * Method:    getAnalogOffset
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AnalogInputJNI_getAnalogOffset
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;

  jint returnValue = HAL_GetAnalogOffset((HAL_AnalogInputHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
