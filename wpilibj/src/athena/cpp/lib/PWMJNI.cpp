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

#include "HAL/DIO.h"
#include "HAL/PWM.h"
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
 * Method:    initializePWMPort
 * Signature: (I)I;
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_PWMJNI_initializePWMPort(
    JNIEnv *env, jclass, jint id) {
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI initializePWMPort";
  PWMJNI_LOG(logDEBUG) << "Port Handle = " << (HalPortHandle)id;
  int32_t status = 0;
  auto pwm = initializePWMPort((HalPortHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << pwm;
  CheckStatus(env, status);
  return (jint)pwm;
}

/*
* Class:     edu_wpi_first_wpilibj_hal_DIOJNI
* Method:    freeDIOPort
* Signature: (I)V;
*/
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_freePWMPort(
    JNIEnv *env, jclass, jint id) {
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI freePWMPort";
  PWMJNI_LOG(logDEBUG) << "Port Handle = " << (HalDigitalHandle)id;
  int32_t status = 0;
  freePWMPort((HalDigitalHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWM
 * Signature: (IS)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWM(
    JNIEnv* env, jclass, jint id, jshort value) {
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HalDigitalHandle)id;
  PWMJNI_LOG(logDEBUG) << "PWM Value = " << value;
  int32_t status = 0;
  setPWM((HalDigitalHandle)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    getPWM
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_wpilibj_hal_PWMJNI_getPWM(JNIEnv* env, jclass, jint id) {
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HalDigitalHandle)id;
  int32_t status = 0;
  jshort returnValue = getPWM((HalDigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    latchPWMZero
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_latchPWMZero(
    JNIEnv* env, jclass, jint id) {
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HalDigitalHandle)id;
  int32_t status = 0;
  latchPWMZero((HalDigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_PWMJNI
 * Method:    setPWMPeriodScale
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_edu_wpi_first_wpilibj_hal_PWMJNI_setPWMPeriodScale(
    JNIEnv* env, jclass, jint id, jint value) {
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HalDigitalHandle)id;
  PWMJNI_LOG(logDEBUG) << "PeriodScale Value = " << value;
  int32_t status = 0;
  setPWMPeriodScale((HalDigitalHandle)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
