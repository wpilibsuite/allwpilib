/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_DIOJNI.h"
#include "hal/DIO.h"
#include "hal/PWM.h"
#include "hal/Ports.h"
#include "hal/cpp/Log.h"
#include "hal/handles/HandlesInternal.h"

using namespace frc;

// set the logging level
TLogLevel dioJNILogLevel = logWARNING;

#define DIOJNI_LOG(level)     \
  if (level > dioJNILogLevel) \
    ;                         \
  else                        \
    Log().Get(level)

extern "C" {

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    initializeDIOPort
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DIOJNI_initializeDIOPort
  (JNIEnv* env, jclass, jint id, jboolean input)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI initializeDIOPort";
  DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_PortHandle)id;
  DIOJNI_LOG(logDEBUG) << "Input = " << (jint)input;
  int32_t status = 0;
  auto dio = HAL_InitializeDIOPort((HAL_PortHandle)id,
                                   static_cast<uint8_t>(input), &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "DIO Handle = " << dio;
  CheckStatusRange(env, status, 0, HAL_GetNumDigitalChannels(),
                   hal::getPortHandleChannel((HAL_PortHandle)id));
  return (jint)dio;
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    checkDIOChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_DIOJNI_checkDIOChannel
  (JNIEnv* env, jclass, jint channel)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI checkDIOChannel";
  DIOJNI_LOG(logDEBUG) << "Channel = " << channel;
  return HAL_CheckDIOChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    freeDIOPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_freeDIOPort
  (JNIEnv* env, jclass, jint id)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI freeDIOPort";
  DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  HAL_FreeDIOPort((HAL_DigitalHandle)id);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    setDIO
 * Signature: (IS)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_setDIO
  (JNIEnv* env, jclass, jint id, jshort value)
{
  // DIOJNI_LOG(logDEBUG) << "Calling DIOJNI setDIO";
  // DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  // DIOJNI_LOG(logDEBUG) << "Value = " << value;
  int32_t status = 0;
  HAL_SetDIO((HAL_DigitalHandle)id, value, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    setDIODirection
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_setDIODirection
  (JNIEnv* env, jclass, jint id, jboolean input)
{
  // DIOJNI_LOG(logDEBUG) << "Calling DIOJNI setDIO";
  // DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  // DIOJNI_LOG(logDEBUG) << "IsInput = " << input;
  int32_t status = 0;
  HAL_SetDIODirection((HAL_DigitalHandle)id, input, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    getDIO
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_DIOJNI_getDIO
  (JNIEnv* env, jclass, jint id)
{
  // DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getDIO";
  // DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  jboolean returnValue = HAL_GetDIO((HAL_DigitalHandle)id, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  // DIOJNI_LOG(logDEBUG) << "getDIOResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    getDIODirection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_DIOJNI_getDIODirection
  (JNIEnv* env, jclass, jint id)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getDIODirection (RR upd)";
  // DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  jboolean returnValue = HAL_GetDIODirection((HAL_DigitalHandle)id, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "getDIODirectionResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    pulse
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_pulse
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI pulse (RR upd)";
  // DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  // DIOJNI_LOG(logDEBUG) << "Value = " << value;
  int32_t status = 0;
  HAL_Pulse((HAL_DigitalHandle)id, value, &status);
  DIOJNI_LOG(logDEBUG) << "Did it work? Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    isPulsing
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_DIOJNI_isPulsing
  (JNIEnv* env, jclass, jint id)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI isPulsing (RR upd)";
  // DIOJNI_LOG(logDEBUG) << "Port Handle = " << (HAL_DigitalHandle)id;
  int32_t status = 0;
  jboolean returnValue = HAL_IsPulsing((HAL_DigitalHandle)id, &status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "isPulsingResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    isAnyPulsing
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_DIOJNI_isAnyPulsing
  (JNIEnv* env, jclass)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI isAnyPulsing (RR upd)";
  int32_t status = 0;
  jboolean returnValue = HAL_IsAnyPulsing(&status);
  // DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "isAnyPulsingResult = " << (jint)returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    getLoopTiming
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL
Java_edu_wpi_first_hal_DIOJNI_getLoopTiming
  (JNIEnv* env, jclass)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI getLoopTimeing";
  int32_t status = 0;
  jshort returnValue = HAL_GetPWMLoopTiming(&status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "LoopTiming = " << returnValue;
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    allocateDigitalPWM
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_DIOJNI_allocateDigitalPWM
  (JNIEnv* env, jclass)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI allocateDigitalPWM";
  int32_t status = 0;
  auto pwm = HAL_AllocateDigitalPWM(&status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  DIOJNI_LOG(logDEBUG) << "PWM Handle = " << pwm;
  CheckStatus(env, status);
  return (jint)pwm;
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    freeDigitalPWM
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_freeDigitalPWM
  (JNIEnv* env, jclass, jint id)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI freeDigitalPWM";
  DIOJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalPWMHandle)id;
  int32_t status = 0;
  HAL_FreeDigitalPWM((HAL_DigitalPWMHandle)id, &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    setDigitalPWMRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_setDigitalPWMRate
  (JNIEnv* env, jclass, jdouble value)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI setDigitalPWMRate";
  DIOJNI_LOG(logDEBUG) << "Rate= " << value;
  int32_t status = 0;
  HAL_SetDigitalPWMRate(value, &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    setDigitalPWMDutyCycle
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_setDigitalPWMDutyCycle
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI setDigitalPWMDutyCycle";
  DIOJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalPWMHandle)id;
  DIOJNI_LOG(logDEBUG) << "DutyCycle= " << value;
  int32_t status = 0;
  HAL_SetDigitalPWMDutyCycle((HAL_DigitalPWMHandle)id, value, &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_DIOJNI
 * Method:    setDigitalPWMOutputChannel
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_DIOJNI_setDigitalPWMOutputChannel
  (JNIEnv* env, jclass, jint id, jint value)
{
  DIOJNI_LOG(logDEBUG) << "Calling DIOJNI setDigitalPWMOutputChannel";
  DIOJNI_LOG(logDEBUG) << "PWM Handle = " << (HAL_DigitalPWMHandle)id;
  DIOJNI_LOG(logDEBUG) << "Channel= " << value;
  int32_t status = 0;
  HAL_SetDigitalPWMOutputChannel((HAL_DigitalPWMHandle)id,
                                 static_cast<uint32_t>(value), &status);
  DIOJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
}

}  // extern "C"
