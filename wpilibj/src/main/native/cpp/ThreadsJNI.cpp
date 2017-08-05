/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_hal_ThreadsJNI.h"

#include "HAL/Threads.h"
#include "HALUtil.h"

using namespace frc;

// set the logging level
TLogLevel threadsJNILogLevel = logWARNING;

#define THREADSJNI_LOG(level)     \
  if (level > threadsJNILogLevel) \
    ;                            \
  else                           \
  Log().Get(level)

extern "C" {
/*
 * Class:     edu_wpi_first_wpilibj_hal_ThreadsJNI
 * Method:    GetCurrentThreadPriority
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_wpi_first_wpilibj_hal_ThreadsJNI_getCurrentThreadPriority
  (JNIEnv *env, jclass) {
  THREADSJNI_LOG(logDEBUG) << "Callling GetCurrentThreadPriority";
  int32_t status = 0;
  HAL_Bool isRT = false;
  auto ret = HAL_GetCurrentThreadPriority(&isRT, &status);
  CheckStatus(env, status);
  return (jint)ret;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_ThreadsJNI
 * Method:    GetCurrentThreadIsRealTime
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_ThreadsJNI_getCurrentThreadIsRealTime
  (JNIEnv *env, jclass) {
  THREADSJNI_LOG(logDEBUG) << "Callling GetCurrentThreadIsRealTime";
  int32_t status = 0;
  HAL_Bool isRT = false;
  HAL_GetCurrentThreadPriority(&isRT, &status);
  CheckStatus(env, status);
  return (jboolean)isRT;
}

/*
 * Class:     edu_wpi_first_wpilibj_hal_ThreadsJNI
 * Method:    SetCurrentThreadPriority
 * Signature: (ZI)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_wpi_first_wpilibj_hal_ThreadsJNI_setCurrentThreadPriority
  (JNIEnv *env, jclass, jboolean realTime, jint priority) {
  THREADSJNI_LOG(logDEBUG) << "Callling SetCurrentThreadPriority";
  int32_t status = 0;
  auto ret = HAL_SetCurrentThreadPriority((HAL_Bool)realTime, (int32_t)priority, &status);
  CheckStatus(env, status);
  return (jboolean)ret;
}

}