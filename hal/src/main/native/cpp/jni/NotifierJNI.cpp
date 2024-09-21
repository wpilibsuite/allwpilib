// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>
#include <cstdio>

#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_NotifierJNI.h"
#include "hal/Notifier.h"

using namespace hal;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    initializeNotifier
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_NotifierJNI_initializeNotifier
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  HAL_NotifierHandle notifierHandle = HAL_InitializeNotifier(&status);

  if (notifierHandle <= 0 || !CheckStatusForceThrow(env, status)) {
    return 0;  // something went wrong in HAL
  }

  return (jint)notifierHandle;
}

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    setHALThreadPriority
 * Signature: (ZI)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_NotifierJNI_setHALThreadPriority
  (JNIEnv* env, jclass, jboolean realTime, jint priority)
{
  int32_t status = 0;
  return HAL_SetNotifierThreadPriority(realTime, priority, &status);
}

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    setNotifierName
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_NotifierJNI_setNotifierName
  (JNIEnv* env, jclass cls, jint notifierHandle, jstring name)
{
  int32_t status = 0;
  HAL_SetNotifierName((HAL_NotifierHandle)notifierHandle,
                      wpi::java::JStringRef{env, name}.c_str(), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    stopNotifier
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_NotifierJNI_stopNotifier
  (JNIEnv* env, jclass cls, jint notifierHandle)
{
  int32_t status = 0;
  HAL_StopNotifier((HAL_NotifierHandle)notifierHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    cleanNotifier
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_NotifierJNI_cleanNotifier
  (JNIEnv* env, jclass, jint notifierHandle)
{
  if (notifierHandle != HAL_kInvalidHandle) {
    HAL_CleanNotifier((HAL_NotifierHandle)notifierHandle);
  }
}

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    updateNotifierAlarm
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_NotifierJNI_updateNotifierAlarm
  (JNIEnv* env, jclass cls, jint notifierHandle, jlong triggerTime)
{
  int32_t status = 0;
  HAL_UpdateNotifierAlarm((HAL_NotifierHandle)notifierHandle,
                          static_cast<uint64_t>(triggerTime), &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    cancelNotifierAlarm
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_NotifierJNI_cancelNotifierAlarm
  (JNIEnv* env, jclass cls, jint notifierHandle)
{
  int32_t status = 0;
  HAL_CancelNotifierAlarm((HAL_NotifierHandle)notifierHandle, &status);
  CheckStatus(env, status);
}

/*
 * Class:     edu_wpi_first_hal_NotifierJNI
 * Method:    waitForNotifierAlarm
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL
Java_edu_wpi_first_hal_NotifierJNI_waitForNotifierAlarm
  (JNIEnv* env, jclass cls, jint notifierHandle)
{
  int32_t status = 0;
  uint64_t time =
      HAL_WaitForNotifierAlarm((HAL_NotifierHandle)notifierHandle, &status);

  CheckStatus(env, status);

  return (jlong)time;
}

}  // extern "C"
