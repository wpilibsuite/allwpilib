/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <assert.h>
#include <jni.h>
#include "HAL/cpp/Log.h"

#include "edu_wpi_first_wpilibj_hal_ConstantsJNI.h"

#include "HAL/Constants.h"
#include "HALUtil.h"

using namespace frc;

// set the logging level
TLogLevel constantsJNILogLevel = logWARNING;

#define CONSTANTSJNI_LOG(level)     \
  if (level > constantsJNILogLevel) \
    ;                         \
  else                        \
  Log().Get(level)

extern "C" {
/*
 * Class:     edu_wpi_first_wpilibj_hal_ConstantsJNI
 * Method:    getSystemClockTicksPerMicrosecond
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_wpi_first_wpilibj_hal_ConstantsJNI_getSystemClockTicksPerMicrosecond(
    JNIEnv *env, jclass) {
  CONSTANTSJNI_LOG(logDEBUG) << "Calling ConstantsJNI getSystemClockTicksPerMicrosecond";
  jint value = HAL_GetSystemClockTicksPerMicrosecond();
  CONSTANTSJNI_LOG(logDEBUG) << "Value = " << value;
  return value;
}
}
