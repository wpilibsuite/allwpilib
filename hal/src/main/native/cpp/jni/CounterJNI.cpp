// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_CounterJNI.h"
#include "hal/Counter.h"
#include "hal/Errors.h"

static_assert(HAL_Counter_Mode::HAL_Counter_kTwoPulse ==
              edu_wpi_first_hal_CounterJNI_TWO_PULSE);

static_assert(HAL_Counter_Mode::HAL_Counter_kSemiperiod ==
              edu_wpi_first_hal_CounterJNI_SEMI_PERIOD);

static_assert(HAL_Counter_Mode::HAL_Counter_kPulseLength ==
              edu_wpi_first_hal_CounterJNI_PULSE_LENGTH);

static_assert(HAL_Counter_Mode::HAL_Counter_kExternalDirection ==
              edu_wpi_first_hal_CounterJNI_EXTERNAL_DIRECTION);

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    initializeCounter
 * Signature: (ILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CounterJNI_initializeCounter
  (JNIEnv* env, jclass, jint mode, jobject index)
{
  jint* indexPtr = reinterpret_cast<jint*>(env->GetDirectBufferAddress(index));
  int32_t status = 0;
  auto counter = HAL_InitializeCounter(
      (HAL_Counter_Mode)mode, reinterpret_cast<int32_t*>(indexPtr), &status);
  CheckStatusForceThrow(env, status);
  return (jint)counter;
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    freeCounter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_freeCounter
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreeCounter((HAL_CounterHandle)id);
  }
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterAverageSize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterAverageSize
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetCounterAverageSize((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterUpSource
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterUpSource
  (JNIEnv* env, jclass, jint id, jint digitalSourceHandle,
   jint analogTriggerType)
{
  int32_t status = 0;
  HAL_SetCounterUpSource((HAL_CounterHandle)id, (HAL_Handle)digitalSourceHandle,
                         (HAL_AnalogTriggerType)analogTriggerType, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterUpSourceEdge
 * Signature: (IZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterUpSourceEdge
  (JNIEnv* env, jclass, jint id, jboolean valueRise, jboolean valueFall)
{
  int32_t status = 0;
  HAL_SetCounterUpSourceEdge((HAL_CounterHandle)id, valueRise, valueFall,
                             &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    clearCounterUpSource
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_clearCounterUpSource
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_ClearCounterUpSource((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterDownSource
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterDownSource
  (JNIEnv* env, jclass, jint id, jint digitalSourceHandle,
   jint analogTriggerType)
{
  int32_t status = 0;
  HAL_SetCounterDownSource((HAL_CounterHandle)id,
                           (HAL_Handle)digitalSourceHandle,
                           (HAL_AnalogTriggerType)analogTriggerType, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterDownSourceEdge
 * Signature: (IZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterDownSourceEdge
  (JNIEnv* env, jclass, jint id, jboolean valueRise, jboolean valueFall)
{
  int32_t status = 0;
  HAL_SetCounterDownSourceEdge((HAL_CounterHandle)id, valueRise, valueFall,
                               &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    clearCounterDownSource
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_clearCounterDownSource
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_ClearCounterDownSource((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterUpDownMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterUpDownMode
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_SetCounterUpDownMode((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterExternalDirectionMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterExternalDirectionMode
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_SetCounterExternalDirectionMode((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterSemiPeriodMode
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterSemiPeriodMode
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  int32_t status = 0;
  HAL_SetCounterSemiPeriodMode((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterPulseLengthMode
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterPulseLengthMode
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_SetCounterPulseLengthMode((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    getCounterSamplesToAverage
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CounterJNI_getCounterSamplesToAverage
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue =
      HAL_GetCounterSamplesToAverage((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterSamplesToAverage
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterSamplesToAverage
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetCounterSamplesToAverage((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    resetCounter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_resetCounter
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  HAL_ResetCounter((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    getCounter
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CounterJNI_getCounter
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jint returnValue = HAL_GetCounter((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    getCounterPeriod
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_CounterJNI_getCounterPeriod
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jdouble returnValue = HAL_GetCounterPeriod((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterMaxPeriod
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterMaxPeriod
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_SetCounterMaxPeriod((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterUpdateWhenEmpty
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterUpdateWhenEmpty
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  int32_t status = 0;
  HAL_SetCounterUpdateWhenEmpty((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    getCounterStopped
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CounterJNI_getCounterStopped
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean returnValue = HAL_GetCounterStopped((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    getCounterDirection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CounterJNI_getCounterDirection
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean returnValue =
      HAL_GetCounterDirection((HAL_CounterHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_CounterJNI
 * Method:    setCounterReverseDirection
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CounterJNI_setCounterReverseDirection
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  int32_t status = 0;
  HAL_SetCounterReverseDirection((HAL_CounterHandle)id, value, &status);
  CheckStatus(env, status);
}

}  // extern "C"
