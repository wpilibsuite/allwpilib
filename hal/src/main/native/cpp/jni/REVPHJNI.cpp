// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_REVPHJNI.h"
#include "hal/REVPH.h"
#include "hal/Ports.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    initialize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_initialize
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto handle = HAL_InitializeREVPH(module, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_free
  (JNIEnv* env, jclass, jint handle)
{
  HAL_FreeREVPH(handle);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    checkSolenoidChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_REVPHJNI_checkSolenoidChannel
  (JNIEnv*, jclass, jint channel)
{
  return HAL_CheckREVPHSolenoidChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getCompressor
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getCompressor
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressor(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setClosedLoopControl
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setClosedLoopControl
  (JNIEnv* env, jclass, jint handle, jboolean enabled)
{
  int32_t status = 0;
  HAL_SetREVPHClosedLoopControl(handle, enabled, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getClosedLoopControl
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getClosedLoopControl
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHClosedLoopControl(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getPressureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getPressureSwitch
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHPressureSwitch(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getAnalogPressure
 * Signature: (II)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getAnalogPressure
  (JNIEnv* env, jclass, jint handle, jint channel)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHAnalogPressure(handle, channel, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getCompressorCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getCompressorCurrent
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHCompressorCurrent(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    getSolenoids
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_REVPHJNI_getSolenoids
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetREVPHSolenoids(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    setSolenoids
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_setSolenoids
  (JNIEnv* env, jclass, jint handle, jint mask, jint value)
{
  int32_t status = 0;
  HAL_SetREVPHSolenoids(handle, mask, value, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_REVPHJNI
 * Method:    fireOneShot
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_REVPHJNI_fireOneShot
  (JNIEnv* env, jclass, jint handle, jint index, jint durMs)
{
  int32_t status = 0;
  HAL_FireREVPHOneShot(handle, index, durMs, &status);
  CheckStatus(env, status, false);
}

}
