/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_PWMJNI.h"
#include "hal/DIO.h"
#include "hal/PWM.h"
#include "hal/Ports.h"
#include "hal/cpp/Log.h"
#include "hal/handles/HandlesInternal.h"

using namespace frc;

// set the logging level
TLogLevel pwmJNILogLevel = logWARNING;

#define PWMJNI_LOG(level)     \
  if (level > pwmJNILogLevel) \
    ;                         \
  else                        \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    initializePWMPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_PWMJNI_initializePWMPort
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI initializePWMPort";
  PWMJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_PortHandle)id;
  int32_t status = 0;
  auto pwm = HAL_InitializePWMPort((HAL_PortHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << pwm;
  CheckStatusRange(env, status, 0, HAL_GetNumPWMChannels(),
                   hal::getPortHandleChannel((HAL_PortHandle)id));
  return (jint)pwm;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    checkPWMChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PWMJNI_checkPWMChannel
  (JNIEnv* env, jclass, jint channel)
{
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI checkPWMChannel";
  PWMJNI_LOG(logDEBUG) << "Channel = " << channel;
  return HAL_CheckPWMChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    freePWMPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_freePWMPort
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI freePWMPort";
  PWMJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  HAL_FreePWMPort((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMConfigRaw
 * Signature: (IIIIII)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMConfigRaw
  (JNIEnv* env, jclass, jint id, jint maxPwm, jint deadbandMaxPwm,
   jint centerPwm, jint deadbandMinPwm, jint minPwm)
{
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMConfigRaw";
  PWMJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  HAL_SetPWMConfigRaw((HAL_DigitalHandle)id, maxPwm, deadbandMaxPwm, centerPwm,
                      deadbandMinPwm, minPwm, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMConfig
 * Signature: (IDDDDD)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMConfig
  (JNIEnv* env, jclass, jint id, jdouble maxPwm, jdouble deadbandMaxPwm,
   jdouble centerPwm, jdouble deadbandMinPwm, jdouble minPwm)
{
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI setPWMConfig";
  PWMJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  HAL_SetPWMConfig((HAL_DigitalHandle)id, maxPwm, deadbandMaxPwm, centerPwm,
                   deadbandMinPwm, minPwm, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMConfigRaw
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMConfigRaw
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "Calling PWMJNI getPWMConfigRaw";
  PWMJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  int32_t maxPwm = 0;
  int32_t deadbandMaxPwm = 0;
  int32_t centerPwm = 0;
  int32_t deadbandMinPwm = 0;
  int32_t minPwm = 0;
  HAL_GetPWMConfigRaw((HAL_DigitalHandle)id, &maxPwm, &deadbandMaxPwm,
                      &centerPwm, &deadbandMinPwm, &minPwm, &status);
  CheckStatus(env, status);
  return CreatePWMConfigDataResult(env, maxPwm, deadbandMaxPwm, centerPwm,
                                   deadbandMinPwm, minPwm);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMEliminateDeadband
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMEliminateDeadband
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  HAL_SetPWMEliminateDeadband((HAL_DigitalHandle)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMEliminateDeadband
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMEliminateDeadband
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  auto val = HAL_GetPWMEliminateDeadband((HAL_DigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
  return (jboolean)val;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMRaw
 * Signature: (IS)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMRaw
  (JNIEnv* env, jclass, jint id, jshort value)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  PWMJNI_LOG(logDEBUG) << "PWM Value = " << value;
  int32_t status = 0;
  HAL_SetPWMRaw((HAL_DigitalHandle)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMSpeed
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMSpeed
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  PWMJNI_LOG(logDEBUG) << "PWM Value = " << value;
  int32_t status = 0;
  HAL_SetPWMSpeed((HAL_DigitalHandle)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMPosition
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMPosition
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  PWMJNI_LOG(logDEBUG) << "PWM Value = " << value;
  int32_t status = 0;
  HAL_SetPWMPosition((HAL_DigitalHandle)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMRaw
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMRaw
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  jshort returnValue = HAL_GetPWMRaw((HAL_DigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMSpeed
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMSpeed
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  jdouble returnValue = HAL_GetPWMSpeed((HAL_DigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    getPWMPosition
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_PWMJNI_getPWMPosition
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  jdouble returnValue = HAL_GetPWMPosition((HAL_DigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  PWMJNI_LOG(logDEBUG) << "Value = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMDisabled
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMDisabled
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  HAL_SetPWMDisabled((HAL_DigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    latchPWMZero
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_latchPWMZero
  (JNIEnv* env, jclass, jint id)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  HAL_LatchPWMZero((HAL_DigitalHandle)id, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_PWMJNI
 * Method:    setPWMPeriodScale
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_PWMJNI_setPWMPeriodScale
  (JNIEnv* env, jclass, jint id, jint value)
{
  PWMJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalHandle)id;
  PWMJNI_LOG(logDEBUG) << "PeriodScale Value = " << value;
  int32_t status = 0;
  HAL_SetPWMPeriodScale((HAL_DigitalHandle)id, value, &status);
  PWMJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
