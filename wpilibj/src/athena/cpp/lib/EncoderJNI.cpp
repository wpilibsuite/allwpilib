/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_EncoderJNI.h"

#include "HAL/Encoder.h"
#include "HAL/Errors.h"
#include "HALUtil.h"

// set the logging level
TLogLevel encoderJNILogLevel = logWARNING;

#define ENCODERJNI_LOG(level)     \
  if (level > encoderJNILogLevel) \
    ;                             \
  else                            \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    initializeEncoder
 * Signature: (BIZBIZZLjava/nio/IntBuffer;)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_initializeEncoder(
    JNIEnv* env, jclass, jbyte port_a_module, jint port_a_pin,
    jboolean port_a_analog_trigger, jbyte port_b_module, jint port_b_pin,
    jboolean port_b_analog_trigger, jboolean reverseDirection, jobject index) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI initializeEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Module A = " << (jint)port_a_module;
  ENCODERJNI_LOG(logDEBUG) << "Pin A = " << port_a_pin;
  ENCODERJNI_LOG(logDEBUG) << "Analog Trigger A = "
                           << (jint)port_a_analog_trigger;
  ENCODERJNI_LOG(logDEBUG) << "Module B = " << (jint)port_b_module;
  ENCODERJNI_LOG(logDEBUG) << "Pin B = " << port_b_pin;
  ENCODERJNI_LOG(logDEBUG) << "Analog Trigger B = "
                           << (jint)port_b_analog_trigger;
  ENCODERJNI_LOG(logDEBUG) << "Reverse direction = " << (jint)reverseDirection;
  jint* indexPtr = (jint*)env->GetDirectBufferAddress(index);
  ENCODERJNI_LOG(logDEBUG) << "Index Ptr = " << indexPtr;
  int32_t status = 0;
  void* encoder = initializeEncoder(
      port_a_module, port_a_pin, port_a_analog_trigger, port_b_module,
      port_b_pin, port_b_analog_trigger, reverseDirection, indexPtr, &status);

  ENCODERJNI_LOG(logDEBUG) << "Index = " << *indexPtr;
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "ENCODER Ptr = " << encoder;
  CheckStatus(env, status);
  return (jlong)encoder;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    freeEncoder
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_freeEncoder(
    JNIEnv* env, jclass, jlong id) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI freeEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  freeEncoder((void*)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    resetEncoder
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_resetEncoder(
    JNIEnv* env, jclass, jlong id) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI resetEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  resetEncoder((void*)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoder
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoder(
    JNIEnv* env, jclass, jlong id) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoder";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  jint returnValue = getEncoder((void*)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderPeriod
 * Signature: (J)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderPeriod(
    JNIEnv* env, jclass, jlong id) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderPeriod";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  double returnValue = getEncoderPeriod((void*)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderPeriodResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderMaxPeriod
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderMaxPeriod(
    JNIEnv* env, jclass, jlong id, jdouble value) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderMaxPeriod";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  setEncoderMaxPeriod((void*)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderStopped
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderStopped(
    JNIEnv* env, jclass, jlong id) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderStopped";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getEncoderStopped((void*)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderStoppedResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderDirection
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderDirection(
    JNIEnv* env, jclass, jlong id) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderDirection";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getEncoderDirection((void*)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderDirectionResult = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderReverseDirection
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderReverseDirection(
    JNIEnv* env, jclass, jlong id, jboolean value) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderReverseDirection";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  setEncoderReverseDirection((void*)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderSamplesToAverage
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderSamplesToAverage(
    JNIEnv* env, jclass, jlong id, jint value) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  setEncoderSamplesToAverage((void*)id, value, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  if (status == PARAMETER_OUT_OF_RANGE) {
    ThrowBoundaryException(env, value, 1, 127);
    return;
  }
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    getEncoderSamplesToAverage
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_getEncoderSamplesToAverage(
    JNIEnv* env, jclass, jlong id) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI getEncoderSamplesToAverage";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  int32_t status = 0;
  jint returnValue = getEncoderSamplesToAverage((void*)id, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  ENCODERJNI_LOG(logDEBUG) << "getEncoderSamplesToAverageResult = "
                           << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_EncoderJNI
 * Method:    setEncoderIndexSource
 * Signature: (JIZZZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_EncoderJNI_setEncoderIndexSource(
    JNIEnv* env, jclass, jlong id, jint pin, jboolean analogTrigger,
    jboolean activeHigh, jboolean edgeSensitive) {
  ENCODERJNI_LOG(logDEBUG) << "Calling ENCODERJNI setEncoderIndexSource";
  ENCODERJNI_LOG(logDEBUG) << "Encoder Ptr = " << (void*)id;
  ENCODERJNI_LOG(logDEBUG) << "Pin = " << pin;
  ENCODERJNI_LOG(logDEBUG) << "Analog Trigger = "
                           << (analogTrigger ? "true" : "false");
  ENCODERJNI_LOG(logDEBUG) << "Active High = "
                           << (activeHigh ? "true" : "false");
  ENCODERJNI_LOG(logDEBUG) << "Edge Sensitive = "
                           << (edgeSensitive ? "true" : "false");
  int32_t status = 0;
  setEncoderIndexSource((void*)id, pin, analogTrigger, activeHigh,
                        edgeSensitive, &status);
  ENCODERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
