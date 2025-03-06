// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_AnalogJNI.h"
#include "hal/AnalogInput.h"
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

}  // extern "C"
