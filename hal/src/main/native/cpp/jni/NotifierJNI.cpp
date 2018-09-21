/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <jni.h>

#include <cassert>
#include <cstdio>

#include "HALUtil.h"
#include "edu_wpi_first_hal_NotifierJNI.h"
#include "hal/Notifier.h"
#include "hal/cpp/Log.h"

using namespace frc;

// set the logging level
TLogLevel notifierJNILogLevel = logWARNING;

#define NOTIFIERJNI_LOG(level)     \
  if (level > notifierJNILogLevel) \
    ;                              \
  else                             \
    Log().Get(level)

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
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI initializeNotifier";

  int32_t status = 0;
  HAL_NotifierHandle notifierHandle = HAL_InitializeNotifier(&status);

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;

  if (notifierHandle <= 0 || !CheckStatusForceThrow(env, status)) {
    return 0;  // something went wrong in HAL
  }

  return (jint)notifierHandle;
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
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI stopNotifier";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  int32_t status = 0;
  HAL_StopNotifier((HAL_NotifierHandle)notifierHandle, &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
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
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI cleanNotifier";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  int32_t status = 0;
  HAL_CleanNotifier((HAL_NotifierHandle)notifierHandle, &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
  CheckStatus(env, status);
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
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI updateNotifierAlarm";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  NOTIFIERJNI_LOG(logDEBUG) << "triggerTime = " << triggerTime;

  int32_t status = 0;
  HAL_UpdateNotifierAlarm((HAL_NotifierHandle)notifierHandle,
                          static_cast<uint64_t>(triggerTime), &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
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
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI cancelNotifierAlarm";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  int32_t status = 0;
  HAL_CancelNotifierAlarm((HAL_NotifierHandle)notifierHandle, &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
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
  NOTIFIERJNI_LOG(logDEBUG) << "Calling NOTIFIERJNI waitForNotifierAlarm";

  NOTIFIERJNI_LOG(logDEBUG) << "Notifier Handle = " << notifierHandle;

  int32_t status = 0;
  uint64_t time =
      HAL_WaitForNotifierAlarm((HAL_NotifierHandle)notifierHandle, &status);
  NOTIFIERJNI_LOG(logDEBUG) << "Status = " << status;
  NOTIFIERJNI_LOG(logDEBUG) << "Time = " << time;
  CheckStatus(env, status);

  return (jlong)time;
}

}  // extern "C"
