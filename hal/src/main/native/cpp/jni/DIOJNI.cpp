// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_DIOJNI.h"
#include "wpi/hal/DIO.h"
#include "wpi/hal/PWM.h"
#include "wpi/hal/Ports.h"
#include "wpi/hal/handles/HandlesInternal.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    initializeDIOPort
 * Signature: (IZ)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_initializeDIOPort
  (JNIEnv* env, jclass, jint channel, jboolean input)
{
  int32_t status = 0;
  auto stack = wpi::util::java::GetJavaStackTrace(env, "org.wpilib");
  auto dio = HAL_InitializeDIOPort(channel, static_cast<uint8_t>(input),
                                   stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return (jint)dio;
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    checkDIOChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_checkDIOChannel
  (JNIEnv* env, jclass, jint channel)
{
  return HAL_CheckDIOChannel(channel);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    freeDIOPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_freeDIOPort
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreeDIOPort((HAL_DigitalHandle)id);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    setDIOSimDevice
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_setDIOSimDevice
  (JNIEnv* env, jclass, jint handle, jint device)
{
  HAL_SetDIOSimDevice((HAL_DigitalHandle)handle, (HAL_SimDeviceHandle)device);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    setDIO
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_setDIO
  (JNIEnv* env, jclass, jint id, jboolean value)
{
  int32_t status = 0;
  HAL_SetDIO((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    setDIODirection
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_setDIODirection
  (JNIEnv* env, jclass, jint id, jboolean input)
{
  int32_t status = 0;
  HAL_SetDIODirection((HAL_DigitalHandle)id, input, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    getDIO
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_getDIO
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean returnValue = HAL_GetDIO((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    getDIODirection
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_getDIODirection
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean returnValue = HAL_GetDIODirection((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    pulse
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_pulse
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_Pulse((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    pulseMultiple
 * Signature: (JD)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_pulseMultiple
  (JNIEnv* env, jclass, jlong channelMask, jdouble value)
{
  int32_t status = 0;
  HAL_PulseMultiple(static_cast<uint32_t>(channelMask), value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    isPulsing
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_isPulsing
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  jboolean returnValue = HAL_IsPulsing((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    isAnyPulsing
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_isAnyPulsing
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  jboolean returnValue = HAL_IsAnyPulsing(&status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    allocateDigitalPWM
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_allocateDigitalPWM
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  auto pwm = HAL_AllocateDigitalPWM(&status);
  CheckStatus(env, status);
  return (jint)pwm;
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    freeDigitalPWM
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_freeDigitalPWM
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreeDigitalPWM((HAL_DigitalPWMHandle)id);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    setDigitalPWMRate
 * Signature: (D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_setDigitalPWMRate
  (JNIEnv* env, jclass, jdouble value)
{
  int32_t status = 0;
  HAL_SetDigitalPWMRate(value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    setDigitalPWMDutyCycle
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_setDigitalPWMDutyCycle
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_SetDigitalPWMDutyCycle((HAL_DigitalPWMHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    setDigitalPWMPPS
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_setDigitalPWMPPS
  (JNIEnv* env, jclass, jint id, jdouble value)
{
  int32_t status = 0;
  HAL_SetDigitalPWMPPS((HAL_DigitalPWMHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_DIOJNI
 * Method:    setDigitalPWMOutputChannel
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_DIOJNI_setDigitalPWMOutputChannel
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetDigitalPWMOutputChannel((HAL_DigitalPWMHandle)id,
                                 static_cast<uint32_t>(value), &status);
  CheckStatus(env, status);
}

}  // extern "C"
