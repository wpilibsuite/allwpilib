// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>
#include <stdint.h>

#include <cassert>

#include "HALUtil.hpp"
#include "org_wpilib_hardware_hal_ThreadsJNI.h"
#include "wpi/hal/Threads.h"

using namespace wpi::hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_ThreadsJNI
 * Method:    getCurrentThreadPriority
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_ThreadsJNI_getCurrentThreadPriority
  (JNIEnv* env, jclass)
{
  int32_t priority = 0;
  HAL_Status status = HAL_GetCurrentThreadPriority(&priority);
  CheckStatus(env, status);
  return static_cast<jint>(priority);
}

/*
 * Class:     org_wpilib_hardware_hal_ThreadsJNI
 * Method:    setCurrentThreadPriority
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_ThreadsJNI_setCurrentThreadPriority
  (JNIEnv* env, jclass, jint priority)
{
  HAL_Status status =
      HAL_SetCurrentThreadPriority(static_cast<int32_t>(priority));
  CheckStatus(env, status, false);
  return static_cast<jboolean>(status);
}

}  // extern "C"
