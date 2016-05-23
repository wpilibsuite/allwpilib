/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "Log.h"

#include "edu_wpi_first_wpilibj_hal_PWMJNI.h"

#include "HAL/Digital.h"
#include "HALUtil.h"

// set the logging level
TLogLevel pwmJNILogLevel = logWARNING;

#define PWMJNI_LOG(level)     \
  if (level > pwmJNILogLevel) \
    ;                         \
  else                        \
  Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    allocatePWMChannel
 * Signature: (J)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_wpilibj_hal_PWMJNI_allocatePWMChannel(
    JNIEnv* env, jclass, jlong id) {
  PWMJNI_LOG(logDEBUG) << "Calling DIOJNI allocatePWMChannel";
  PWMJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  int32_t status = 0;
  jboolean returnValue = allocatePWMChannel((void*)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "allocatePWMChannelResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    freePWMChannel
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_freePWMChannel(
    JNIEnv* env, jclass, jlong id) {
  PWMJNI_LOG(logDEBUG) << "Calling DIOJNI freePWMChannel";
  PWMJNI_LOG(logDEBUG) << "Port Ptr = " << (void*)id;
  int32_t status = 0;
  freePWMChannel((void*)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWM
 * Signature: (JS)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWM(
    JNIEnv* env, jclass, jlong id, jshort value) {
  PWMJNI_LOG(logDEBUG) << "DigitalPort Ptr = " << (void*)id;
  PWMJNI_LOG(logDEBUG) << "PWM Value = " << value;
  int32_t status = 0;
  setPWM((void*)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    getPWM
 * Signature: (J)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_wpilibj_hal_PWMJNI_getPWM(JNIEnv* env, jclass, jlong id) {
  PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << (void*)id;
  int32_t status = 0;
  jshort returnValue = getPWM((void*)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    latchPWMZero
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_latchPWMZero(
    JNIEnv* env, jclass, jlong id) {
  PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << (void*)id;
  int32_t status = 0;
  latchPWMZero((void*)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMPeriodScale
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMPeriodScale(
    JNIEnv* env, jclass, jlong id, jint value) {
  PWMJNI_LOG(logDEBUG) << "DigitalPort Ptr = " << (void*)id;
  PWMJNI_LOG(logDEBUG) << "PeriodScale Value = " << value;
  int32_t status = 0;
  setPWMPeriodScale((void*)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    allocatePWM
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_wpilibj_hal_PWMJNI_allocatePWM(JNIEnv* env, jclass) {
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI allocatePWM";
  int32_t status = 0;
  void* pwm = allocatePWM(&status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << pwm;
  CheckStatus(env, status);
  return (jlong)pwm;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    freePWM
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_PWMJNI_freePWM(JNIEnv* env, jclass, jlong id) {
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI freePWM";
  PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << (void*)id;
  int32_t status = 0;
  freePWM((void*)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMRate(
    JNIEnv* env, jclass, jdouble value) {
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMRate";
  PWMJNI_LOG(logDEBUG) << "Rate= " << value;
  int32_t status = 0;
  setPWMRate(value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMDutyCycle
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMDutyCycle(
    JNIEnv* env, jclass, jlong id, jdouble value) {
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMDutyCycle";
  PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << (void*)id;
  PWMJNI_LOG(logDEBUG) << "DutyCycle= " << value;
  int32_t status = 0;
  setPWMDutyCycle((void*)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMOutputChannel
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMOutputChannel(
    JNIEnv* env, jclass, jlong id, jint value) {
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMOutputChannel";
  PWMJNI_LOG(logDEBUG) << "PWM Ptr = " << (void*)id;
  PWMJNI_LOG(logDEBUG) << "Pin= " << value;
  int32_t status = 0;
  setPWMOutputChannel((void*)id, (uint32_t)value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
