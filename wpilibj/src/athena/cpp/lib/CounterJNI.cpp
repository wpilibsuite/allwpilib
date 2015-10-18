/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_CounterJNI.h"

#include "HAL/Digital.hpp"
#include "HAL/Errors.hpp"
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
 * Signature: (ILjava/nio/IntBuffer;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_initializeCounter(
    JNIEnv* env, jclass, jint mode, jobject index) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI initializeCounter";
  COUNTERJNI_LOG(logDEBUG) << "Mode = " << mode;
  jint* indexPtr = (jint*)env->GetDirectBufferAddress(index);
  COUNTERJNI_LOG(logDEBUG) << "Index Ptr = " << (uint32_t*)indexPtr;
  int32_t status = 0;
  void* counter = initializeCounter((Mode)mode, (uint32_t*)indexPtr, &status);
  COUNTERJNI_LOG(logDEBUG) << "Index = " << *indexPtr;
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "COUNTER Ptr = " << counter;
  CheckStatus(env, status);
  return (jlong)counter;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    freeCounter
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_freeCounter(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI freeCounter";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  freeCounter((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterAverageSize
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterAverageSize(
    JNIEnv* env, jclass, jlong id, jint value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterAverageSize";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "AverageSize = " << value;
  int32_t status = 0;
  setCounterAverageSize((void*)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpSource
 * Signature: (JIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpSource(
    JNIEnv* env, jclass, jlong id, jint pin, jboolean analogTrigger) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "Pin = " << pin;
  COUNTERJNI_LOG(logDEBUG) << "AnalogTrigger = " << (jint)analogTrigger;
  int32_t status = 0;
  setCounterUpSource((void*)id, pin, analogTrigger, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpSourceEdge
 * Signature: (JZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpSourceEdge(
    JNIEnv* env, jclass, jlong id, jboolean valueRise, jboolean valueFall) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpSourceEdge";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "Rise = " << (jint)valueRise;
  COUNTERJNI_LOG(logDEBUG) << "Fall = " << (jint)valueFall;
  int32_t status = 0;
  setCounterUpSourceEdge((void*)id, valueRise, valueFall, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    clearCounterUpSource
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_clearCounterUpSource(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI clearCounterUpSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  clearCounterUpSource((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterDownSource
 * Signature: (JIZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterDownSource(
    JNIEnv* env, jclass, jlong id, jint pin, jboolean analogTrigger) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterDownSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "Pin = " << pin;
  COUNTERJNI_LOG(logDEBUG) << "AnalogTrigger = " << (jint)analogTrigger;
  int32_t status = 0;
  setCounterDownSource((void*)id, pin, analogTrigger, &status);
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
 * Signature: (JZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterDownSourceEdge(
    JNIEnv* env, jclass, jlong id, jboolean valueRise, jboolean valueFall) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterDownSourceEdge";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "Rise = " << (jint)valueRise;
  COUNTERJNI_LOG(logDEBUG) << "Fall = " << (jint)valueFall;
  int32_t status = 0;
  setCounterDownSourceEdge((void*)id, valueRise, valueFall, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    clearCounterDownSource
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_clearCounterDownSource(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI clearCounterDownSource";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  clearCounterDownSource((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpDownMode
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpDownMode(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterUpDownMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  setCounterUpDownMode((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterExternalDirectionMode
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterExternalDirectionMode(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG)
      << "Calling COUNTERJNI setCounterExternalDirectionMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  setCounterExternalDirectionMode((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterSemiPeriodMode
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterSemiPeriodMode(
    JNIEnv* env, jclass, jlong id, jboolean value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterSemiPeriodMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "SemiPeriodMode = " << (jint)value;
  int32_t status = 0;
  setCounterSemiPeriodMode((void*)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterPulseLengthMode
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterPulseLengthMode(
    JNIEnv* env, jclass, jlong id, jdouble value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterPulseLengthMode";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "PulseLengthMode = " << value;
  int32_t status = 0;
  setCounterPulseLengthMode((void*)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterSamplesToAverage
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterSamplesToAverage(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterSamplesToAverage";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  jint returnValue = getCounterSamplesToAverage((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterSamplesToAverage
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterSamplesToAverage(
    JNIEnv* env, jclass, jlong id, jint value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterSamplesToAverage";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "SamplesToAverage = " << value;
  int32_t status = 0;
  setCounterSamplesToAverage((void*)id, value, &status);
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
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_resetCounter(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI resetCounter";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  resetCounter((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounter
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounter(
    JNIEnv* env, jclass, jlong id) {
  // COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounter";
  // COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  jint returnValue = getCounter((void*)id, &status);
  // COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  // COUNTERJNI_LOG(logDEBUG) << "getCounterResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterPeriod
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterPeriod(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterPeriod";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  jdouble returnValue = getCounterPeriod((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterPeriodResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterMaxPeriod
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterMaxPeriod(
    JNIEnv* env, jclass, jlong id, jdouble value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterMaxPeriod";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "MaxPeriod = " << value;
  int32_t status = 0;
  setCounterMaxPeriod((void*)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterUpdateWhenEmpty
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterUpdateWhenEmpty(
    JNIEnv* env, jclass, jlong id, jboolean value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterMaxPeriod";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "UpdateWhenEmpty = " << (jint)value;
  int32_t status = 0;
  setCounterUpdateWhenEmpty((void*)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterStopped
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterStopped(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterStopped";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getCounterStopped((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterStoppedResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    getCounterDirection
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_getCounterDirection(
    JNIEnv* env, jclass, jlong id) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI getCounterDirection";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getCounterDirection((void*)id, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  COUNTERJNI_LOG(logDEBUG) << "getCounterDirectionResult = "
                           << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_CounterJNI
 * Method:    setCounterReverseDirection
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_CounterJNI_setCounterReverseDirection(
    JNIEnv* env, jclass, jlong id, jboolean value) {
  COUNTERJNI_LOG(logDEBUG) << "Calling COUNTERJNI setCounterReverseDirection";
  COUNTERJNI_LOG(logDEBUG) << "Counter Ptr = " << (void*)id;
  COUNTERJNI_LOG(logDEBUG) << "ReverseDirection = " << (jint)value;
  int32_t status = 0;
  setCounterReverseDirection((void*)id, value, &status);
  COUNTERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
