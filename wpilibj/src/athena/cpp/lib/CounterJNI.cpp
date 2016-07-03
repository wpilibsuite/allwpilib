/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_CounterJNI.h"

#include "HAL/Counter.h"
#include "HAL/Errors.h"
#include "HALUtil.h"

// set the logging level
TLogLevel counterJNILogLevel = logWARNING;

#define COUNTERJNI_LOG(level)     \
  if (level > counterJNILogLevel) \
    ;                             \
  else                            \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    initializeCounter
 * Signature: (ILjava/nio/IntBuffer;)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_initializeCounter(
    JNIEnv* env, jclass, jint mode, jobject index) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI initializeCounter";
  COUNTERJNI_LOG(logDEBUG) << "Mode = " << mode;
  jint* indexPtr = (jint*)env->GetDirectBufferAddress(index);
  COUNTERJNI_LOG(logDEBUG) << "Index Ptr = " << (int32_t*)indexPtr;
  int32_t status = 0;
  auto counter = initializeCounter((Mode)mode, (int32_t*)indexPtr, &status);
  COUNTERJNI_LOG(logDEBUG) << "Index = " << *indexPtr;
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "COUNTER Handle = " << counter;
  CheckStatus(env, status);
  return (jint)counter;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    freeCounter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_freeCounter(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI freeCounter";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  freeCounter((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterAverageSize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterAverageSize(
    JNIEnv* env, jclass, jint id, jint value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterAverageSize";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "AverageSize = " << value;
  int32_t status = 0;
  setCounterAverageSize((HalCounterHandle)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpSource
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpSource(
    JNIEnv* env, jclass, jint id, jint pin, jboolean analogTrigger) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "Pin = " << pin;
  COUNTERJNI_LOG(logDEBUG) << "AnalogTrigger = " << (jint)analogTrigger;
  int32_t status = 0;
  setCounterUpSource((HalCounterHandle)id, pin, analogTrigger, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpSourceEdge
 * Signature: (IZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpSourceEdge(
    JNIEnv* env, jclass, jint id, jboolean valueRise, jboolean valueFall) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpSourceEdge";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "Rise = " << (jint)valueRise;
  COUNTERJNI_LOG(logDEBUG) << "Fall = " << (jint)valueFall;
  int32_t status = 0;
  setCounterUpSourceEdge((HalCounterHandle)id, valueRise, valueFall, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    clearCounterUpSource
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_clearCounterUpSource(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI clearCounterUpSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  clearCounterUpSource((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterDownSource
 * Signature: (IIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterDownSource(
    JNIEnv* env, jclass, jint id, jint pin, jboolean analogTrigger) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterDownSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "Pin = " << pin;
  COUNTERJNI_LOG(logDEBUG) << "AnalogTrigger = " << (jint)analogTrigger;
  int32_t status = 0;
  setCounterDownSource((HalCounterHandle)id, pin, analogTrigger, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  if (status == PARAMETER_OUT_OF_RANGE) {
    ThrowIllegalArgumentException(env,
                                  "Counter only supports DownSource in "
                                  "TwoPulse and ExternalDirection modes.");
    return;
  }
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterDownSourceEdge
 * Signature: (IZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterDownSourceEdge(
    JNIEnv* env, jclass, jint id, jboolean valueRise, jboolean valueFall) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterDownSourceEdge";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "Rise = " << (jint)valueRise;
  COUNTERJNI_LOG(logDEBUG) << "Fall = " << (jint)valueFall;
  int32_t status = 0;
  setCounterDownSourceEdge((HalCounterHandle)id, valueRise, valueFall, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    clearCounterDownSource
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_clearCounterDownSource(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI clearCounterDownSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  clearCounterDownSource((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpDownMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpDownMode(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpDownMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  setCounterUpDownMode((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterExternalDirectionMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterExternalDirectionMode(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG)
      << "Calling COUNTERJNI setCounterExternalDirectionMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  setCounterExternalDirectionMode((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterSemiPeriodMode
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterSemiPeriodMode(
    JNIEnv* env, jclass, jint id, jboolean value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterSemiPeriodMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "SemiPeriodMode = " << (jint)value;
  int32_t status = 0;
  setCounterSemiPeriodMode((HalCounterHandle)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterPulseLengthMode
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterPulseLengthMode(
    JNIEnv* env, jclass, jint id, jdouble value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterPulseLengthMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "PulseLengthMode = " << value;
  int32_t status = 0;
  setCounterPulseLengthMode((HalCounterHandle)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterSamplesToAverage
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterSamplesToAverage(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterSamplesToAverage";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  jint returnValue = getCounterSamplesToAverage((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterSamplesToAverage
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterSamplesToAverage(
    JNIEnv* env, jclass, jint id, jint value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterSamplesToAverage";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "SamplesToAverage = " << value;
  int32_t status = 0;
  setCounterSamplesToAverage((HalCounterHandle)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  if (status == PARAMETER_OUT_OF_RANGE) {
    ThrowBoundaryException(env, value, 1, 127);
    return;
  }
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    resetCounter
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_resetCounter(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI resetCounter";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  resetCounter((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounter
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounter(
    JNIEnv* env, jclass, jint id) {
  // COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounter";
  // COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  jint returnValue = getCounter((HalCounterHandle)id, &status);
  // COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  // COUNTERJNI_LOG(logDEBUG) << "getCounterResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterPeriod
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterPeriod(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterPeriod";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  jdouble returnValue = getCounterPeriod((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterPeriodResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterMaxPeriod
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterMaxPeriod(
    JNIEnv* env, jclass, jint id, jdouble value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterMaxPeriod";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "MaxPeriod = " << value;
  int32_t status = 0;
  setCounterMaxPeriod((HalCounterHandle)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpdateWhenEmpty
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpdateWhenEmpty(
    JNIEnv* env, jclass, jint id, jboolean value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterMaxPeriod";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "UpdateWhenEmpty = " << (jint)value;
  int32_t status = 0;
  setCounterUpdateWhenEmpty((HalCounterHandle)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterStopped
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterStopped(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterStopped";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  jboolean returnValue = getCounterStopped((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterStoppedResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterDirection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterDirection(
    JNIEnv* env, jclass, jint id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterDirection";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  int32_t status = 0;
  jboolean returnValue = getCounterDirection((HalCounterHandle)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterDirectionResult = "
                           << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterReverseDirection
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterReverseDirection(
    JNIEnv* env, jclass, jint id, jboolean value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterReverseDirection";
  COUNTERJNI_LOG(logDEBUG) << "Counter Handle = " << (HalCounterHandle)id;
  COUNTERJNI_LOG(logDEBUG) << "ReverseDirection = " << (jint)value;
  int32_t status = 0;
  setCounterReverseDirection((HalCounterHandle)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
