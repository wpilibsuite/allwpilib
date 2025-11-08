// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include <wpi/jni_util.h>
#include <wpi/string.h>

#include "org_wpilib_hardware_hal_UsageReportingJNI.h"
#include "hal/UsageReporting.h"

using namespace wpi::java;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_UsageReportingJNI
 * Method:    report
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_UsageReportingJNI_report
  (JNIEnv* env, jclass, jstring resource, jstring data)
{
  JStringRef resourceStr{env, resource};
  JStringRef dataStr{env, data};
  WPI_String resourceWpiStr = wpi::make_string(resourceStr);
  WPI_String dataWpiStr = wpi::make_string(dataStr);
  return HAL_ReportUsage(&resourceWpiStr, &dataWpiStr);
}

}  // extern "C"
