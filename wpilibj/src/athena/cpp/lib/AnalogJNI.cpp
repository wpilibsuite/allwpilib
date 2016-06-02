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
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogInputPort(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HalPortHandle)id;
  int32_t status = 0;
  void *analog = initializeAnalogInputPort((HalPortHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << analog;
  CheckStatus(env, status);
  return (jlong)analog;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    freeAnalogInputPort
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_freeAnalogInputPort(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Ptr = " << (void *)id;
  freeAnalogInputPort((void *)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initializeAnalogOutputPort
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogOutputPort(
    JNIEnv *env, jclass, jint id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Handle = " << (HalPortHandle)id;
  int32_t status = 0;
  void *analog = initializeAnalogOutputPort((HalPortHandle)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << analog;
  CheckStatus(env, status);
  return (jlong)analog;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    freeAnalogOutputPort
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_freeAnalogOutputPort(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Port Ptr = " << (void *)id;
  freeAnalogOutputPort((void *)id);
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
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogOutput(
    JNIEnv *env, jclass, jlong id, jdouble voltage) {
  ANALOGJNI_LOG(logDEBUG) << "Calling setAnalogOutput";
  ANALOGJNI_LOG(logDEBUG) << "Voltage = " << voltage;
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  setAnalogOutput((void *)id, voltage, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOutput
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOutput(
    JNIEnv *env, jclass, jlong id) {
  int32_t status = 0;
  double val = getAnalogOutput((void *)id, &status);
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
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogAverageBits(
    JNIEnv *env, jclass, jlong id, jint value) {
  ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << value;
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  setAnalogAverageBits((void *)id, value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageBits
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageBits(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  jint returnValue = getAnalogAverageBits((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageBits = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogOversampleBits
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogOversampleBits(
    JNIEnv *env, jclass, jlong id, jint value) {
  ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << value;
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  setAnalogOversampleBits((void *)id, value, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOversampleBits
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOversampleBits(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  jint returnValue = getAnalogOversampleBits((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "OversampleBits = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogValue
 * Signature: (J)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogValue(
    JNIEnv *env, jclass, jlong id) {
  // ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void*)id;
  int32_t status = 0;
  jshort returnValue = getAnalogValue((void *)id, &status);
  // ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  // ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageValue
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageValue(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  jint returnValue = getAnalogAverageValue((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageValue = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogVoltsToValue
 * Signature: (JD)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogVoltsToValue(
    JNIEnv *env, jclass, jlong id, jdouble voltageValue) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  ANALOGJNI_LOG(logDEBUG) << "VoltageValue = " << voltageValue;
  int32_t status = 0;
  jint returnValue = getAnalogVoltsToValue((void *)id, voltageValue, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogVoltage
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogVoltage(
    JNIEnv *env, jclass, jlong id) {
  // ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void*)id;
  int32_t status = 0;
  jdouble returnValue = getAnalogVoltage((void *)id, &status);
  // ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  // ANALOGJNI_LOG(logDEBUG) << "Voltage = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogAverageVoltage
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogAverageVoltage(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  jdouble returnValue = getAnalogAverageVoltage((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AverageVoltage = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogLSBWeight
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogLSBWeight(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  jint returnValue = getAnalogLSBWeight((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogLSBWeight = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogOffset
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogOffset(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  jint returnValue = getAnalogOffset((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    isAccumulatorChannel
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_isAccumulatorChannel(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "isAccumulatorChannel";
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  jboolean returnValue = isAccumulatorChannel((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogOffset = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    initAccumulator
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initAccumulator(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;
  initAccumulator((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    resetAccumulator
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_resetAccumulator(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  resetAccumulator((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAccumulatorCenter
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAccumulatorCenter(
    JNIEnv *env, jclass, jlong id, jint center) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  setAccumulatorCenter((void *)id, center, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAccumulatorDeadband
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAccumulatorDeadband(
    JNIEnv *env, jclass, jlong id, jint deadband) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  setAccumulatorDeadband((void *)id, deadband, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorValue
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorValue(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  jlong returnValue = getAccumulatorValue((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AccumulatorValue = " << returnValue;
  CheckStatus(env, status);

  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorCount
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorCount(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  jint returnValue = getAccumulatorCount((void *)id, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AccumulatorCount = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAccumulatorOutput
 * Signature: (JLjava/nio/LongBuffer;Ljava/nio/IntBuffer;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAccumulatorOutput(
    JNIEnv *env, jclass, jlong id, jobject value, jobject count) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Ptr = " << (void *)id;
  int32_t status = 0;

  jlong *valuePtr = (jlong *)env->GetDirectBufferAddress(value);
  uint32_t *countPtr = (uint32_t *)env->GetDirectBufferAddress(count);

  getAccumulatorOutput((void *)id, valuePtr, countPtr, &status);

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
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_initializeAnalogTrigger(
    JNIEnv *env, jclass, jint id, jobject index) {
  ANALOGJNI_LOG(logDEBUG) << "Port Ptr = " << (HalPortHandle)id;

  jint *indexPtr = (jint *)env->GetDirectBufferAddress(index);
  ANALOGJNI_LOG(logDEBUG) << "Index Ptr = " << indexPtr;

  int32_t status = 0;
  void *analogTrigger =
      initializeAnalogTrigger((HalPortHandle)id, (uint32_t *)indexPtr, &status);
  ANALOGJNI_LOG(logDEBUG) << "Status = " << status;
  ANALOGJNI_LOG(logDEBUG) << "AnalogTrigger Ptr = " << analogTrigger;
  CheckStatus(env, status);
  return (jlong)analogTrigger;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    cleanAnalogTrigger
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_cleanAnalogTrigger(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  cleanAnalogTrigger((void *)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsRaw
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerLimitsRaw(
    JNIEnv *env, jclass, jlong id, jint lower, jint upper) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  setAnalogTriggerLimitsRaw((void *)id, lower, upper, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerLimitsVoltage
 * Signature: (JDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerLimitsVoltage(
    JNIEnv *env, jclass, jlong id, jdouble lower, jdouble upper) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  setAnalogTriggerLimitsVoltage((void *)id, lower, upper, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerAveraged
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerAveraged(
    JNIEnv *env, jclass, jlong id, jboolean averaged) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  setAnalogTriggerAveraged((void *)id, averaged, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    setAnalogTriggerFiltered
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_setAnalogTriggerFiltered(
    JNIEnv *env, jclass, jlong id, jboolean filtered) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  setAnalogTriggerFiltered((void *)id, filtered, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerInWindow
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerInWindow(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  jboolean val = getAnalogTriggerInWindow((void *)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerTriggerState
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerTriggerState(
    JNIEnv *env, jclass, jlong id) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  jboolean val = getAnalogTriggerTriggerState((void *)id, &status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_AnalogJNI
 * Method:    getAnalogTriggerOutput
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_AnalogJNI_getAnalogTriggerOutput(
    JNIEnv *env, jclass, jlong id, jint type) {
  ANALOGJNI_LOG(logDEBUG) << "Analog Trigger Ptr = " << (void *)id;

  int32_t status = 0;
  jboolean val =
      getAnalogTriggerOutput((void *)id, (AnalogTriggerType)type, &status);
  CheckStatus(env, status);
  return val;
}

}  // extern "C"
