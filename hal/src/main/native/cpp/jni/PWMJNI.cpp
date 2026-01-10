// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_PWMJNI.h"
#include "wpi/hal/DIO.h"
#include "wpi/hal/PWM.h"
#include "wpi/hal/Ports.h"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/util/jni_util.hpp"

using namespace wpi::hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_PWMJNI
 * Method:    initializePWMPort
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PWMJNI_initializePWMPort
  (JNIEnv* env, jclass, jint channel)
{
  int32_t status = 0;
  auto stack = wpi::util::java::GetJavaStackTrace(env, "org.wpilib");
  auto pwm = HAL_InitializePWMPort(channel, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return (jint)pwm;
}

/*
 * Class:     org_wpilib_hardware_hal_PWMJNI
 * Method:    checkPWMChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_PWMJNI_checkPWMChannel
  (JNIEnv* env, jclass, jint channel)
{
  return HAL_CheckPWMChannel(channel);
}

/*
 * Class:     org_wpilib_hardware_hal_PWMJNI
 * Method:    freePWMPort
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_PWMJNI_freePWMPort
  (JNIEnv* env, jclass, jint id)
{
  if (id != HAL_kInvalidHandle) {
    HAL_FreePWMPort((HAL_DigitalHandle)id);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_PWMJNI
 * Method:    setPWMSimDevice
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_PWMJNI_setPWMSimDevice
  (JNIEnv* env, jclass, jint handle, jint device)
{
  HAL_SetPWMSimDevice((HAL_DigitalHandle)handle, (HAL_SimDeviceHandle)device);
}

/*
 * Class:     org_wpilib_hardware_hal_PWMJNI
 * Method:    setPulseTimeMicroseconds
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_PWMJNI_setPulseTimeMicroseconds
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetPWMPulseTimeMicroseconds((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_PWMJNI
 * Method:    getPulseTimeMicroseconds
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_PWMJNI_getPulseTimeMicroseconds
  (JNIEnv* env, jclass, jint id)
{
  int32_t status = 0;
  int32_t returnValue =
      HAL_GetPWMPulseTimeMicroseconds((HAL_DigitalHandle)id, &status);
  CheckStatus(env, status);
  return returnValue;
}

/*
 * Class:     org_wpilib_hardware_hal_PWMJNI
 * Method:    setPWMOutputPeriod
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_PWMJNI_setPWMOutputPeriod
  (JNIEnv* env, jclass, jint id, jint value)
{
  int32_t status = 0;
  HAL_SetPWMOutputPeriod((HAL_DigitalHandle)id, value, &status);
  CheckStatus(env, status);
}

}  // extern "C"
