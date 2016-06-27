/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_AnalogJNI.h"

#include "HAL/AnalogInput.h"
#include "HAL/AnalogOutput.h"
#include "HAL/AnalogAccumulator.h"
#include "HAL/AnalogTrigger.h"
#include "HALUtil.h"

// set the logging level
TLogLevel analogJNILogLevel = logWARNING;

#define ANALOGJNI_LOG(level)     \
  if (level > analogJNILogLevel) \
    ;                            \
  else                           \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initializeAnalogInputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogInputPort(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HalPortHandle)id;
  int32_t status = 0;
  auto analog = initializeAnalogInputPort((HalPortHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << analog;
  CheckStatus(env, status);
  return (jint)analog;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    freeAnalogInputPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_freeAnalogInputPort(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HalAnalogInputHandle)id;
  freeAnalogInputPort((HalAnalogInputHandle)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initializeAnalogOutputPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogOutputPort(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HalPortHandle)id;
  int32_t status = 0;
  HalAnalogOutputHandle analog = initializeAnalogOutputPort((HalPortHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << analog;
  CheckStatus(env, status);
  return (jlong)analog;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    freeAnalogOutputPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_freeAnalogOutputPort(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << id;
  freeAnalogOutputPort((HalAnalogOutputHandle)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    checkAnalogModule
 * Signature: (B)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_checkAnalogModule(
    JNIEnv *, jclass, jbyte value) {
  // ANALOGJNI_LOG(logDEBUG) << "Module = " << (jint)value;
  jboolean returnValue = checkAnalogModule(value);
  // ANALOGJNI_LOG(logDEBUG) << "checkAnalogModuleResult = " <<
  // (jint)returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    checkAnalogInputChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_checkAnalogInputChannel(
    JNIEnv *, jclass, jint value) {
  // ANALOGJNI_LOG(logDEBUG) << "Channel = " << value;
  jboolean returnValue = checkAnalogInputChannel(value);
  // ANALOGJNI_LOG(logDEBUG) << "checkAnalogChannelResult = " <<
  // (jint)returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    checkAnalogOutputChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_checkAnalogOutputChannel(
    JNIEnv *, jclass, jint value) {
  // ANALOGJNI_LOG(logDEBUG) << "Channel = " << value;
  jboolean returnValue = checkAnalogOutputChannel(value);
  // ANALOGJNI_LOG(logDEBUG) << "checkAnalogChannelResult = " <<
  // (jint)returnValue;
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogOutput
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogOutput(
    JNIEnv *env, jclass, jint id, jdouble voltage) {
  ANALOGJNI_LOG(logDEBUG) << "Calling setAnalogOutput";
  ANALOGJNI_LOG(logDEBUG) << "Voltage = " << voltage;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << id;
  int32_t status = 0;
  setAnalogOutput((HalAnalogOutputHandle)id, voltage, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOutput
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOutput(
    JNIEnv *env, jclass, jint id) {
  int32_t status = 0;
  double val = getAnalogOutput((HalAnalogOutputHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogSampleRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogSampleRate(
    JNIEnv *env, jclass, jdouble value) {
  ANALOGJNI_LOG(logDEBUG) << "SampleRate = " << value;
  int32_t status = 0;
  setAnalogSampleRate(value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogSampleRate
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogSampleRate(
    JNIEnv *env, jclass) {
  int32_t status = 0;
  double returnValue = getAnalogSampleRate(&status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "SampleRate = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogAverageBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogAverageBits(
    JNIEnv *env, jclass, jint id, jint value) {
  ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << value;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  setAnalogAverageBits((HalAnalogInputHandle)id, value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageBits(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  jint returnValue = getAnalogAverageBits((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogOversampleBits
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogOversampleBits(
    JNIEnv *env, jclass, jint id, jint value) {
  ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << value;
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  setAnalogOversampleBits((HalAnalogInputHandle)id, value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOversampleBits
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOversampleBits(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  jint returnValue = getAnalogOversampleBits((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogValue
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogValue(
    JNIEnv *env, jclass, jint id) {
  // ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (void*)id;
  int32_t status = 0;
  jshort returnValue = getAnalogValue((HalAnalogInputHandle)id, &status);
  // ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  // ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageValue
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageValue(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  jint returnValue = getAnalogAverageValue((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageValue = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogVoltsToValue
 * Signature: (ID)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogVoltsToValue(
    JNIEnv *env, jclass, jint id, jdouble voltageValue) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  ANALOGJNI_LOG(logDEBUG) << "VoltageValue = " << voltageValue;
  int32_t status = 0;
  jint returnValue = getAnalogVoltsToValue((HalAnalogInputHandle)id, voltageValue, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogVoltage(
    JNIEnv *env, jclass, jint id) {
  // ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (void*)id;
  int32_t status = 0;
  jdouble returnValue = getAnalogVoltage((HalAnalogInputHandle)id, &status);
  // ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  // ANALOGJNI_LOG(logDEBUG) << "Voltage = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageVoltage
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageVoltage(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  jdouble returnValue = getAnalogAverageVoltage((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageVoltage = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogLSBWeight
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogLSBWeight(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  jint returnValue = getAnalogLSBWeight((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogLSBWeight = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOffset
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOffset(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  jint returnValue = getAnalogOffset((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    isAccumulatorChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_isAccumulatorChannel(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "isAccumulatorChannel";
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  jboolean returnValue = isAccumulatorChannel((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initAccumulator
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initAccumulator(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;
  initAccumulator((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    resetAccumulator
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_resetAccumulator(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  resetAccumulator((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAccumulatorCenter
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAccumulatorCenter(
    JNIEnv *env, jclass, jint id, jint center) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  setAccumulatorCenter((HalAnalogInputHandle)id, center, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAccumulatorDeadband
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAccumulatorDeadband(
    JNIEnv *env, jclass, jint id, jint deadband) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  setAccumulatorDeadband((HalAnalogInputHandle)id, deadband, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorValue
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorValue(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  jlong returnValue = getAccumulatorValue((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AccumulatorValue = " << returnValue;
  CheckStatus(env, status);

  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorCount(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  jint returnValue = getAccumulatorCount((HalAnalogInputHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AccumulatorCount = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorOutput
 * Signature: (ILjava/nio/LongBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorOutput(
    JNIEnv *env, jclass, jint id, jobject value, jobject count) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Handle = " << (HalAnalogInputHandle)id;
  int32_t status = 0;

  jlong *valuePtr = (jlong *)env->GetDirectBufferAddress(value);
  uint32_t *countPtr = (uint32_t *)env->GetDirectBufferAddress(count);

  getAccumulatorOutput((HalAnalogInputHandle)id, valuePtr, countPtr, &status);

  ANALOGJNI_LOG(logDEBUG) << "Value = " << *valuePtr;
  ANALOGJNI_LOG(logDEBUG) << "Count = " << *countPtr;
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initializeAnalogTrigger
 * Signature: (ILjava/nio/IntBuffer;)J
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogTrigger(
    JNIEnv *env, jclass, jint id, jobject index) {
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HalAnalogInputHandle)id;

  jint *indexHandle = (jint *)env->GetDirectBufferAddress(index);
  ANALOGJNI_LOG(logDEBUG) << "Index Ptr = " << indexHandle;

  int32_t status = 0;
  HalAnalogTriggerHandle analogTrigger =
      initializeAnalogTrigger((HalAnalogInputHandle)id, (uint32_t *)indexHandle, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogTrigger Handle = " << analogTrigger;
  CheckStatus(env, status);
  return (jint)analogTrigger;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    cleanAnalogTrigger
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_cleanAnalogTrigger(
    JNIEnv *env, jclass,jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  cleanAnalogTrigger((HalAnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsRaw
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerLimitsRaw(
    JNIEnv *env, jclass,jint id, jint lower, jint upper) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  setAnalogTriggerLimitsRaw((HalAnalogTriggerHandle)id, lower, upper, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsVoltage
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerLimitsVoltage(
    JNIEnv *env, jclass,jint id, jdouble lower, jdouble upper) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  setAnalogTriggerLimitsVoltage((HalAnalogTriggerHandle)id, lower, upper, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerAveraged
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerAveraged(
    JNIEnv *env, jclass,jint id, jboolean averaged) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  setAnalogTriggerAveraged((HalAnalogTriggerHandle)id, averaged, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerFiltered
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerFiltered(
    JNIEnv *env, jclass,jint id, jboolean filtered) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  setAnalogTriggerFiltered((HalAnalogTriggerHandle)id, filtered, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerInWindow
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerInWindow(
    JNIEnv *env, jclass,jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  jboolean val = getAnalogTriggerInWindow((HalAnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerTriggerState
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerTriggerState(
    JNIEnv *env, jclass,jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  jboolean val = getAnalogTriggerTriggerState((HalAnalogTriggerHandle)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerOutput
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerOutput(
    JNIEnv *env, jclass,jint id, jint type) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Handle = " << (HalAnalogTriggerHandle)id;

  int32_t status = 0;
  jboolean val =
      getAnalogTriggerOutput((HalAnalogTriggerHandle)id, (AnalogTriggerType)type, &status);
  CheckStatus(env, status);
  return val;
}

}  // extern "C"
