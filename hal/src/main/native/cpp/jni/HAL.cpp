// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/HAL.h"

#include <jni.h>

#ifdef __FRC_ROBORIO__
#include <signal.h>
#endif

#include <cassert>
#include <cstring>

#include <fmt/format.h>
#include <wpi/jni_util.h>

#include "HALUtil.h"
#include "edu_wpi_first_hal_HAL.h"
#include "hal/DriverStation.h"
#include "hal/Main.h"

using namespace hal;
using namespace wpi::java;

extern "C" {

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    initialize
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_initialize
  (JNIEnv*, jclass, jint timeout, jint mode)
{
  return HAL_Initialize(timeout, mode);
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    shutdown
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_shutdown
  (JNIEnv*, jclass)
{
  return HAL_Shutdown();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    hasMain
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_hasMain
  (JNIEnv*, jclass)
{
  return HAL_HasMain();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    runMain
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_runMain
  (JNIEnv*, jclass)
{
  HAL_RunMain();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    exitMain
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_exitMain
  (JNIEnv*, jclass)
{
  HAL_ExitMain();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    terminate
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_terminate
  (JNIEnv*, jclass)
{
#ifdef __FRC_ROBORIO__
  ::raise(SIGKILL);
#endif
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    simPeriodicBeforeNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_simPeriodicBeforeNative
  (JNIEnv*, jclass)
{
  HAL_SimPeriodicBefore();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    simPeriodicAfterNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_wpi_first_hal_HAL_simPeriodicAfterNative
  (JNIEnv*, jclass)
{
  HAL_SimPeriodicAfter();
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getSystemActive
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_getSystemActive
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetSystemActive(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getBrownedOut
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_getBrownedOut
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetBrownedOut(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getCommsDisableCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getCommsDisableCount
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  int32_t val = HAL_GetCommsDisableCount(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getRSLState
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_getRSLState
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetRSLState(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getSystemTimeValid
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_wpi_first_hal_HAL_getSystemTimeValid
  (JNIEnv* env, jclass)
{
  int32_t status = 0;
  bool val = HAL_GetSystemTimeValid(&status);
  CheckStatus(env, status);
  return val;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getPortWithModule
 * Signature: (BB)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getPortWithModule
  (JNIEnv* env, jclass, jbyte module, jbyte channel)
{
  HAL_PortHandle port = HAL_GetPortWithModule(module, channel);
  return (jint)port;
}

/*
 * Class:     edu_wpi_first_hal_HAL
 * Method:    getPort
 * Signature: (B)I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_hal_HAL_getPort
  (JNIEnv* env, jclass, jbyte channel)
{
  HAL_PortHandle port = HAL_GetPort(channel);
  return (jint)port;
}

}  // extern "C"
