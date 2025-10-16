// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_ConstantsJNI.h"
#include "wpi/hal/Constants.h"

using namespace wpi::hal;

extern "C" {
/*
 * Class:     org_wpilib_hardware_hal_ConstantsJNI
 * Method:    getSystemClockTicksPerMicrosecond
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_ConstantsJNI_getSystemClockTicksPerMicrosecond
  (JNIEnv* env, jclass)
{
  jint value = HAL_GetSystemClockTicksPerMicrosecond();
  return value;
}
}  // extern "C"
