// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "edu_wpi_first_hal_ThreadsJNI.h"
#include "hal/Threads.h"

using namespace hal;

extern "C" {
/*
 * Class:     edu_wpi_first_hal_ThreadsJNI
 * Method:    getCurrentThreadPriority
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_ThreadsJNI_getCurrentThreadPriority
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  HAL_Bool isRT = false;
  auto ret = HAL_GetCurrentThreadPriority(&isRT, &status);
  CheckStatus(env, status);
  return static_cast<jint>(ret);
}

/*
 * Class:     edu_wpi_first_hal_ThreadsJNI
 * Method:    getCurrentThreadIsRealTime
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_ThreadsJNI_getCurrentThreadIsRealTime
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  HAL_Bool isRT = false;
  HAL_GetCurrentThreadPriority(&isRT, &status);
  CheckStatus(env, status);
  return static_cast<jboolean>(isRT);
}

/*
 * Class:     edu_wpi_first_hal_ThreadsJNI
 * Method:    setCurrentThreadPriority
 * Signature: (ZI)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_ThreadsJNI_setCurrentThreadPriority
  (JNIEnv* env, jclass, jboolean realTime, jint priority)
{
  int32_t status = 0;
  auto ret = HAL_SetCurrentThreadPriority(
      static_cast<HAL_Bool>(realTime), static_cast<int32_t>(priority), &status);
  CheckStatus(env, status, false);
  return static_cast<jboolean>(ret);
}

}  // extern "C"
