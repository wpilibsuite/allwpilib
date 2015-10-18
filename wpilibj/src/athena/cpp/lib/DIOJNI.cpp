/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.hpp"

#include "edu_wpi_first_wpilibj_hal_DIOJNI.h"

#include "HAL/Digital.hpp"
#include "HALUtil.h"

// set the logging level
TLogLevel dioJNILogLevel = logWARNING;

#define DIOJNI_LOG(level)     \
  if (level > dioJNILogLevel) \
    ;                         \
  else                        \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    initializeDigitalPort
 * Signature: (J)J;
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_DIOJNI_initializeDigitalPort(
    JNIEnv *env, jclass, jlong id) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI initializeDigitalPort";
  DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void *)id;
  int32_t status = 0;
  void *dio = initializeDigitalPort((void *)id, &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "DIO Ptr = " << dio;
  CheckStatus(env, status);
  return (jlong)dio;
}

/*
* Class:     edu_wpi_first_wpilibj_hal_DIOJNI
* Method:    freeDigitalPort
* Signature: (J)V;
*/
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_freeDigitalPort(
    JNIEnv *env, jclass, jlong id) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI freeDigitalPort";
  DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void *)id;
  freeDigitalPort((void *)id);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    allocateDIO
 * Signature: (JZ)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_allocateDIO(
    JNIEnv *env, jclass, jlong id, jboolean value) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI allocateDIO";
  DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void *)id;
  int32_t status = 0;
  jboolean returnValue = allocateDIO((void *)id, value, &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "allocateDIOResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    freeDIO
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_DIOJNI_freeDIO(JNIEnv *env, jclass, jlong id) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI freeDIO";
  DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void *)id;
  int32_t status = 0;
  freeDIO((void *)id, &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    setDIO
 * Signature: (JS)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_setDIO(
    JNIEnv *env, jclass, jlong id, jshort value) {
  // DIOJNI_LOG(logDEBUG) << "Calling DIOJNI setDIO";
  // DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  // DIOJNI_LOG(logDEBUG) << "Value = " << value;
  int32_t status = 0;
  setDIO((void *)id, value, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    getDIO
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_DIOJNI_getDIO(JNIEnv *env, jclass, jlong id) {
  // DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getDIO";
  // DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getDIO((void *)id, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  // DIOJNI_LOG(logDEBUG) << "getDIOResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    getDIODirection
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_DIOJNI_getDIODirection(
    JNIEnv *env, jclass, jlong id) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getDIODirection (RR upd)";
  // DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = getDIODirection((void *)id, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "getDIODirectionResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    pulse
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_DIOJNI_pulse(
    JNIEnv *env, jclass, jlong id, jdouble value) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI pulse (RR upd)";
  // DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  // DIOJNI_LOG(logDEBUG) << "Value = " << value;
  int32_t status = 0;
  pulse((void *)id, value, &status);
  DIOJNI_LOG(logDEBUG) << "Did it work? Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    isPulsing
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_DIOJNI_isPulsing(JNIEnv *env, jclass, jlong id) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI isPulsing (RR upd)";
  // DIOJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = isPulsing((void *)id, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "isPulsingResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    isAnyPulsing
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_DIOJNI_isAnyPulsing(JNIEnv *env, jclass) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI isAnyPulsing (RR upd)";
  int32_t status = 0;
  jboolean returnValue = isAnyPulsing(&status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "isAnyPulsingResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_DIOJNI
 * Method:    getLoopTiming
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_wpilibj_hal_DIOJNI_getLoopTiming(JNIEnv *env, jclass) {
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getLoopTimeing";
  int32_t status = 0;
  jshort returnValue = getLoopTiming(&status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "LoopTiming = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

}  // extern "C"
