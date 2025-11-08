// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_CounterJNI.h"
#include "wpi/hal/Counter.h"
#include "wpi/hal/Errors.h"
#include "wpi/util/jni_util.hpp"

using namespace hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    initializeCounter
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_initializeCounter
  (JNIEnv* env, jclass, jint channel, jboolean risingEdge)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto counter =
      HAL_InitializeCounter(channel, risingEdge, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return (jint)counter;
}

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    freeCounter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_freeCounter
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreeCounter((HAL_CounterHandle)id);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    setCounterEdgeConfiguration
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_setCounterEdgeConfiguration
  (JNIEnv* env, jclass, jint id, jboolean valueRise)
{
  int32_t status = 0;
  HAL_SetCounterEdgeConfiguration((HAL_CounterHandle)id, valueRise, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    resetCounter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_resetCounter
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_ResetCounter((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    getCounter
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_getCounter
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue = HAL_GetCounter((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    getCounterPeriod
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_getCounterPeriod
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue = HAL_GetCounterPeriod((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    setCounterMaxPeriod
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_setCounterMaxPeriod
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_SetCounterMaxPeriod((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_CounterJNI
 * Method:    getCounterStopped
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_CounterJNI_getCounterStopped
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean returnValue = HAL_GetCounterStopped((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
