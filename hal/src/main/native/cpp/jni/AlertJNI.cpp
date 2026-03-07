// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>
#include <cstdio>

#include "HALUtil.hpp"
#include "org_wpilib_hardware_hal_AlertJNI.h"
#include "wpi/hal/Alert.h"
#include "wpi/util/jni_util.hpp"
#include "wpi/util/string.hpp"

using namespace wpi::hal;
using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_AlertJNI
 * Method:    createAlert
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_AlertJNI_createAlert
  (JNIEnv* env, jclass, jstring group, jstring text, jint level)
{
  int32_t status = 0;
  wpi::util::java::JStringRef jgroup{env, group};
  wpi::util::java::JStringRef jtext{env, text};
  WPI_String wpiGroup = wpi::util::make_string(jgroup);
  WPI_String wpiText = wpi::util::make_string(jtext);
  HAL_AlertHandle alertHandle =
      HAL_CreateAlert(&wpiGroup, &wpiText, level, &status);

  if (alertHandle <= 0 || !CheckStatusForceThrow(env, status)) {
    return 0;  // something went wrong in HAL
  }

  return (jint)alertHandle;
}

/*
 * Class:     org_wpilib_hardware_hal_AlertJNI
 * Method:    destroyAlert
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AlertJNI_destroyAlert
  (JNIEnv* env, jclass, jint alertHandle)
{
  if (alertHandle != HAL_kInvalidHandle) {
    HAL_DestroyAlert((HAL_AlertHandle)alertHandle);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_AlertJNI
 * Method:    setAlertActive
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AlertJNI_setAlertActive
  (JNIEnv* env, jclass cls, jint alertHandle, jboolean active)
{
  int32_t status = 0;
  HAL_SetAlertActive((HAL_AlertHandle)alertHandle, active, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AlertJNI
 * Method:    isAlertActive
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_AlertJNI_isAlertActive
  (JNIEnv* env, jclass cls, jint alertHandle)
{
  int32_t status = 0;
  jboolean active = HAL_IsAlertActive((HAL_AlertHandle)alertHandle, &status);
  CheckStatus(env, status);
  return active;
}

/*
 * Class:     org_wpilib_hardware_hal_AlertJNI
 * Method:    setAlertText
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_AlertJNI_setAlertText
  (JNIEnv* env, jclass cls, jint alertHandle, jstring text)
{
  int32_t status = 0;
  wpi::util::java::JStringRef jtext{env, text};
  WPI_String wpiText = wpi::util::make_string(jtext);
  HAL_SetAlertText((HAL_AlertHandle)alertHandle, &wpiText, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_AlertJNI
 * Method:    getAlertText
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_org_wpilib_hardware_hal_AlertJNI_getAlertText
  (JNIEnv* env, jclass cls, jint alertHandle)
{
  int32_t status = 0;
  WPI_String text;
  HAL_GetAlertText((HAL_AlertHandle)alertHandle, &text, &status);
  if (!CheckStatus(env, status)) {
    return nullptr;
  }
  jstring rv = MakeJString(env, wpi::util::to_string_view(&text));
  WPI_FreeString(&text);
  return rv;
}

}  // extern "C"
