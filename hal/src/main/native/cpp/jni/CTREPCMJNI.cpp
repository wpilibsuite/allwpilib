// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_CTREPCMJNI.h"
#include "hal/CTREPCM.h"
#include "hal/Ports.h"
#include "hal/handles/HandlesInternal.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    initialize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_initialize
  (JNIEnv* env, jclass, jint module)
{
  int32_t status = 0;
  auto stack = wpi::java::GetJavaStackTrace(env, "edu.wpi.first");
  auto handle = HAL_InitializeCTREPCM(module, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return handle;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_free
  (JNIEnv* env, jclass, jint handle)
{
  if (handle != HAL_kInvalidHandle) {
    HAL_FreeCTREPCM(handle);
  }
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    checkSolenoidChannel
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_checkSolenoidChannel
  (JNIEnv*, jclass, jint channel)
{
  return HAL_CheckCTREPCMSolenoidChannel(channel);
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressor
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressor
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressor(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    setClosedLoopControl
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_setClosedLoopControl
  (JNIEnv* env, jclass, jint handle, jboolean enabled)
{
  int32_t status = 0;
  HAL_SetCTREPCMClosedLoopControl(handle, enabled, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getClosedLoopControl
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getClosedLoopControl
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMClosedLoopControl(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getPressureSwitch
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getPressureSwitch
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMPressureSwitch(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressorCurrent
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressorCurrent
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorCurrent(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressorCurrentTooHighFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressorCurrentTooHighFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorCurrentTooHighFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressorCurrentTooHighStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressorCurrentTooHighStickyFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result =
      HAL_GetCTREPCMCompressorCurrentTooHighStickyFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressorShortedFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressorShortedFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorShortedFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressorShortedStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressorShortedStickyFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorShortedStickyFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressorNotConnectedFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressorNotConnectedFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMCompressorNotConnectedFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getCompressorNotConnectedStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getCompressorNotConnectedStickyFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result =
      HAL_GetCTREPCMCompressorNotConnectedStickyFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getSolenoids
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getSolenoids
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoids(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    setSolenoids
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_setSolenoids
  (JNIEnv* env, jclass, jint handle, jint mask, jint value)
{
  int32_t status = 0;
  HAL_SetCTREPCMSolenoids(handle, mask, value, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getSolenoidDisabledList
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getSolenoidDisabledList
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidDisabledList(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getSolenoidVoltageFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getSolenoidVoltageFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidVoltageFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    getSolenoidVoltageStickyFault
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_getSolenoidVoltageStickyFault
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetCTREPCMSolenoidVoltageStickyFault(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    clearAllStickyFaults
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_clearAllStickyFaults
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ClearAllCTREPCMStickyFaults(handle, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    fireOneShot
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_fireOneShot
  (JNIEnv* env, jclass, jint handle, jint index)
{
  int32_t status = 0;
  HAL_FireCTREPCMOneShot(handle, index, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     edu_wpi_first_hal_CTREPCMJNI
 * Method:    setOneShotDuration
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_CTREPCMJNI_setOneShotDuration
  (JNIEnv* env, jclass, jint handle, jint index, jint durMs)
{
  int32_t status = 0;
  HAL_SetCTREPCMOneShotDuration(handle, index, durMs, &status);
  CheckStatus(env, status, false);
}
}  // extern "C"
