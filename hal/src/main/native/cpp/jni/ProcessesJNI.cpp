// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_ProcessesJNI.h"
#include "hal/Processes.h"

using namespace hal;
using namespace wpi::java;

extern "C" {
/*
 * Class:     edu_wpi_first_hal_ProcessesJNI
 * Method:    setProcessPriority
 * Signature: (Ljava/lang/String;ZI)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_ProcessesJNI_setProcessPriority
  (JNIEnv* env, jclass, jstring process, jboolean realTime, jint priority)
{
  int32_t status = 0;
  auto ret = HAL_SetProcessPriority(JStringRef{env, process}.c_str(),
                                    static_cast<HAL_Bool>(realTime),
                                    static_cast<int32_t>(priority), &status);
  CheckStatus(env, status);
  return static_cast<jboolean>(ret);
}

}  // extern "C"
