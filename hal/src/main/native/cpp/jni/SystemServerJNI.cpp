// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "org_wpilib_hardware_hal_SystemServerJNI.h"
#include "wpi/hal/SystemServer.h"

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_SystemServerJNI
 * Method:    getSystemServerHandle
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_SystemServerJNI_getSystemServerHandle
  (JNIEnv*, jclass)
{
  return HAL_GetSystemServerHandle();
}

}  // extern "C"
