// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>
#include <cstdio>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_NotifierJNI.h"
#include "wpi/hal/Notifier.h"
#include "wpi/util/jni_util.hpp"
#include "wpi/util/string.h"

using namespace wpi::hal;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    createNotifier
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_createNotifier
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  HAL_NotifierHandle notifierHandle = HAL_CreateNotifier(&status);

  if (notifierHandle <= 0 || !CheckStatusForceThrow(env, status)) {
    return 0;  // something went wrong in HAL
  }

  return (jint)notifierHandle;
}

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    setHALThreadPriority
 * Signature: (ZI)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_setHALThreadPriority
  (JNIEnv* env, jclass, jboolean realTime, jint priority)
{
  int32_t status = 0;
  return HAL_SetNotifierThreadPriority(realTime, priority, &status);
}

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    setNotifierName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_setNotifierName
  (JNIEnv* env, jclass cls, jint notifierHandle, jstring name)
{
  int32_t status = 0;
  wpi::util::java::JStringRef jname{env, name};
  WPI_String wpiName = wpi::util::make_string(jname);
  HAL_SetNotifierName((HAL_NotifierHandle)notifierHandle, &wpiName, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    destroyNotifier
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_destroyNotifier
  (JNIEnv* env, jclass, jint notifierHandle)
{
  if (notifierHandle != HAL_kInvalidHandle) {
    HAL_DestroyNotifier((HAL_NotifierHandle)notifierHandle);
  }
}

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    setNotifierAlarm
 * Signature: (IJJZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_setNotifierAlarm
  (JNIEnv* env, jclass cls, jint notifierHandle, jlong alarmTime,
   jlong intervalTime, jboolean absolute)
{
  int32_t status = 0;
  HAL_SetNotifierAlarm((HAL_NotifierHandle)notifierHandle,
                       static_cast<uint64_t>(alarmTime),
                       static_cast<uint64_t>(intervalTime), absolute, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    cancelNotifierAlarm
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_cancelNotifierAlarm
  (JNIEnv* env, jclass cls, jint notifierHandle)
{
  int32_t status = 0;
  HAL_CancelNotifierAlarm((HAL_NotifierHandle)notifierHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    acknowledgeNotifierAlarm
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_acknowledgeNotifierAlarm
  (JNIEnv* env, jclass cls, jint notifierHandle)
{
  int32_t status = 0;
  HAL_AcknowledgeNotifierAlarm((HAL_NotifierHandle)notifierHandle, &status);

  CheckStatus(env, status);
}

/*
 * Class:     org_wpilib_hardware_hal_NotifierJNI
 * Method:    getNotifierOverrun
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_NotifierJNI_getNotifierOverrun
  (JNIEnv* env, jclass cls, jint notifierHandle)
{
  int32_t status = 0;
  int32_t count =
      HAL_GetNotifierOverrun((HAL_NotifierHandle)notifierHandle, &status);

  CheckStatus(env, status);

  return (jint)count;
}

}  // extern "C"
