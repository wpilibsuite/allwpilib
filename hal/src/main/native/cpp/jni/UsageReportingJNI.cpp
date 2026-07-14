// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <cassert>

#include "org_wpilib_hardware_hal_UsageReportingJNI.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/util/jni_util.hpp"
#include "wpi/util/string.h"

using namespace wpi::util::java;

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_UsageReportingJNI
 * Method:    report
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_UsageReportingJNI_report
  (JNIEnv* env, jclass, jstring resource, jstring data)
{
  JStringRef resourceStr{env, resource};
  JStringRef dataStr{env, data};
  WPI_String resourceWpiStr = wpi::util::make_string(resourceStr);
  WPI_String dataWpiStr = wpi::util::make_string(dataStr);
  HAL_ReportUsage(&resourceWpiStr, &dataWpiStr);
}

/*
 * Class:     org_wpilib_hardware_hal_UsageReportingJNI
 * Method:    publishCanVersion
 * Signature: (IILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_UsageReportingJNI_publishCanVersion(
    JNIEnv* env, jclass, jint busId, jint deviceId, jstring name,
    jstring version) {
  JStringRef nameStr{env, name};
  JStringRef versionStr{env, version};
  WPI_String nameWpiStr = wpi::util::make_string(nameStr);
  WPI_String versionWpiStr = wpi::util::make_string(versionStr);
  return HAL_PublishCanVersion(static_cast<uint8_t>(busId),
                               static_cast<uint32_t>(deviceId), &nameWpiStr,
                               &versionWpiStr);
}

/*
 * Class:     org_wpilib_hardware_hal_UsageReportingJNI
 * Method:    publishVersion
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_UsageReportingJNI_publishVersion(
    JNIEnv* env, jclass, jstring name, jstring version) {
  JStringRef nameStr{env, name};
  JStringRef versionStr{env, version};
  WPI_String nameWpiStr = wpi::util::make_string(nameStr);
  WPI_String versionWpiStr = wpi::util::make_string(versionStr);
  return HAL_PublishVersion(&nameWpiStr, &versionWpiStr);
}

/*
 * Class:     org_wpilib_hardware_hal_UsageReportingJNI
 * Method:    publishWpilibVersion
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_UsageReportingJNI_publishWpilibVersion(
    JNIEnv* env, jclass, jstring version) {
  JStringRef versionStr{env, version};
  WPI_String versionWpiStr = wpi::util::make_string(versionStr);
  return HAL_PublishWpilibVersion(&versionWpiStr);
}

}  // extern "C"
