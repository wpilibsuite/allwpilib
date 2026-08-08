// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include "HALUtil.hpp"
#include "org_wpilib_hardware_hal_A301JNI.h"
#include "wpi/hal/A301.h"
#include "wpi/util/jni_util.hpp"

using namespace wpi::hal;

static_assert(org_wpilib_hardware_hal_A301JNI_MIN_DEVICE_ID ==
              HAL_A301_MIN_DEVICE_ID);
static_assert(org_wpilib_hardware_hal_A301JNI_MAX_DEVICE_ID ==
              HAL_A301_MAX_DEVICE_ID);
static_assert(org_wpilib_hardware_hal_A301JNI_DEFAULT_DEVICE_ID ==
              HAL_A301_DEFAULT_DEVICE_ID);
static_assert(org_wpilib_hardware_hal_A301JNI_STATUS_0 == HAL_A301_STATUS_0);
static_assert(org_wpilib_hardware_hal_A301JNI_STATUS_1 == HAL_A301_STATUS_1);
static_assert(org_wpilib_hardware_hal_A301JNI_STATUS_2 == HAL_A301_STATUS_2);
static_assert(org_wpilib_hardware_hal_A301JNI_STATUS_3 == HAL_A301_STATUS_3);
static_assert(org_wpilib_hardware_hal_A301JNI_GEARBOX_RPM_UNKNOWN ==
              HAL_A301_GEARBOX_RPM_UNKNOWN);
static_assert(org_wpilib_hardware_hal_A301JNI_GEARBOX_RPM_215 ==
              HAL_A301_GEARBOX_RPM_215);
static_assert(org_wpilib_hardware_hal_A301JNI_GEARBOX_RPM_500 ==
              HAL_A301_GEARBOX_RPM_500);
static_assert(org_wpilib_hardware_hal_A301JNI_IDLE_MODE_COAST ==
              HAL_A301_IDLE_MODE_COAST);
static_assert(org_wpilib_hardware_hal_A301JNI_IDLE_MODE_BRAKE ==
              HAL_A301_IDLE_MODE_BRAKE);
static_assert(org_wpilib_hardware_hal_A301JNI_CONTROL_TYPE_DUTY_CYCLE ==
              HAL_A301_CONTROL_TYPE_DUTY_CYCLE);
static_assert(org_wpilib_hardware_hal_A301JNI_CONTROL_TYPE_VELOCITY ==
              HAL_A301_CONTROL_TYPE_VELOCITY);
static_assert(org_wpilib_hardware_hal_A301JNI_CONTROL_TYPE_VOLTAGE ==
              HAL_A301_CONTROL_TYPE_VOLTAGE);
static_assert(org_wpilib_hardware_hal_A301JNI_CONTROL_TYPE_RELATIVE_POSITION ==
              HAL_A301_CONTROL_TYPE_RELATIVE_POSITION);
static_assert(org_wpilib_hardware_hal_A301JNI_CONTROL_TYPE_ABSOLUTE_POSITION ==
              HAL_A301_CONTROL_TYPE_ABSOLUTE_POSITION);
static_assert(org_wpilib_hardware_hal_A301JNI_CONTROL_TYPE_CURRENT ==
              HAL_A301_CONTROL_TYPE_CURRENT);

namespace {

jobject MakeFirmwareVersion(JNIEnv* env,
                            const HAL_A301FirmwareVersion& version) {
  jclass cls = env->FindClass("org/wpilib/hardware/hal/A301FirmwareVersion");
  if (!cls) {
    return nullptr;
  }
  jmethodID constructor = env->GetMethodID(cls, "<init>", "(IIIIII)V");
  jobject result =
      env->NewObject(cls, constructor, version.major, version.minor,
                     version.patch, version.prerelease,
                     version.hardwareRevision, static_cast<jint>(version.raw));
  env->DeleteLocalRef(cls);
  return result;
}

jobject MakePeriodicStatus0(JNIEnv* env, const HAL_A301PeriodicStatus0& frame,
                            int32_t status) {
  jclass cls = env->FindClass("org/wpilib/hardware/hal/A301PeriodicStatus0");
  if (!cls) {
    return nullptr;
  }
  jmethodID constructor = env->GetMethodID(cls, "<init>", "(DDDIZZIIJ)V");
  jobject result = env->NewObject(
      cls, constructor, frame.appliedOutput, frame.voltage, frame.current,
      frame.motorTemperature, static_cast<jboolean>(frame.inverted),
      static_cast<jboolean>(frame.primaryHeartbeatLock),
      static_cast<jint>(frame.gearboxRPM), status,
      static_cast<jlong>(frame.timestamp));
  env->DeleteLocalRef(cls);
  return result;
}

jobject MakePeriodicStatus1(JNIEnv* env, const HAL_A301PeriodicStatus1& frame,
                            int32_t status) {
  int32_t faults = frame.otherFault | (frame.motorTypeFault << 1) |
                   (frame.sensorFault << 2) | (frame.canFault << 3) |
                   (frame.temperatureFault << 4) | (frame.drvFault << 5) |
                   (frame.escEepromFault << 6) | (frame.firmwareFault << 7) |
                   (frame.motorStartupFault << 8);
  int32_t warnings =
      frame.brownoutWarning | (frame.overcurrentWarning << 1) |
      (frame.escEepromWarning << 2) | (frame.extEepromWarning << 3) |
      (frame.sensorWarning << 4) | (frame.stallWarning << 5) |
      (frame.hasResetWarning << 6) | (frame.otherWarning << 7) |
      (frame.overvoltageWarning << 8) | (frame.motorLoopSpeedWarning << 9);
  int32_t stickyFaults =
      frame.otherStickyFault | (frame.motorTypeStickyFault << 1) |
      (frame.sensorStickyFault << 2) | (frame.canStickyFault << 3) |
      (frame.temperatureStickyFault << 4) | (frame.drvStickyFault << 5) |
      (frame.escEepromStickyFault << 6) | (frame.firmwareStickyFault << 7) |
      (frame.motorStartupStickyFault << 8);
  int32_t stickyWarnings =
      frame.brownoutStickyWarning | (frame.overcurrentStickyWarning << 1) |
      (frame.escEepromStickyWarning << 2) |
      (frame.extEepromStickyWarning << 3) | (frame.sensorStickyWarning << 4) |
      (frame.stallStickyWarning << 5) | (frame.hasResetStickyWarning << 6) |
      (frame.otherStickyWarning << 7) | (frame.overvoltageStickyWarning << 8) |
      (frame.motorLoopSpeedStickyWarning << 9);

  jclass cls = env->FindClass("org/wpilib/hardware/hal/A301PeriodicStatus1");
  if (!cls) {
    return nullptr;
  }
  jmethodID constructor = env->GetMethodID(cls, "<init>", "(IIIIZIJ)V");
  jobject result =
      env->NewObject(cls, constructor, faults, warnings, stickyFaults,
                     stickyWarnings, static_cast<jboolean>(frame.isFollower),
                     status, static_cast<jlong>(frame.timestamp));
  env->DeleteLocalRef(cls);
  return result;
}

jobject MakePeriodicStatus2(JNIEnv* env, const HAL_A301PeriodicStatus2& frame,
                            int32_t status) {
  jclass cls = env->FindClass("org/wpilib/hardware/hal/A301PeriodicStatus2");
  if (!cls) {
    return nullptr;
  }
  jmethodID constructor = env->GetMethodID(cls, "<init>", "(DDIJ)V");
  jobject result = env->NewObject(cls, constructor, frame.encoderVelocity,
                                  frame.relativeEncoderPosition, status,
                                  static_cast<jlong>(frame.timestamp));
  env->DeleteLocalRef(cls);
  return result;
}

jobject MakePeriodicStatus3(JNIEnv* env, const HAL_A301PeriodicStatus3& frame,
                            int32_t status) {
  jclass cls = env->FindClass("org/wpilib/hardware/hal/A301PeriodicStatus3");
  if (!cls) {
    return nullptr;
  }
  jmethodID constructor = env->GetMethodID(cls, "<init>", "(DIJ)V");
  jobject result =
      env->NewObject(cls, constructor, frame.absoluteEncoderPosition, status,
                     static_cast<jlong>(frame.timestamp));
  env->DeleteLocalRef(cls);
  return result;
}

}  // namespace

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    detectDeviceId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_A301JNI_detectDeviceId
  (JNIEnv* env, jclass, jint busId)
{
  int32_t status = 0;
  int32_t deviceId = HAL_DetectA301DeviceId(busId, &status);
  CheckStatusForceThrow(env, status);
  return deviceId;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    initialize
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_A301JNI_initialize
  (JNIEnv* env, jclass, jint busId, jint deviceId)
{
  int32_t status = 0;
  auto stack = wpi::util::java::GetJavaStackTrace(env, "org.wpilib");
  HAL_A301Handle handle =
      HAL_InitializeA301(busId, deviceId, stack.c_str(), &status);
  CheckStatusForceThrow(env, status);
  return handle;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    free
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_free
  (JNIEnv*, jclass, jint handle)
{
  HAL_FreeA301(handle);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    checkDeviceId
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_A301JNI_checkDeviceId
  (JNIEnv*, jclass, jint deviceId)
{
  return HAL_CheckA301DeviceId(deviceId);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getBusId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getBusId
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  int32_t result = HAL_GetA301BusId(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getDeviceId
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getDeviceId
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  int32_t result = HAL_GetA301DeviceId(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getFirmwareVersion
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getFirmwareVersion
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_A301FirmwareVersion version{};
  HAL_GetA301FirmwareVersion(handle, &version, &status);
  CheckStatus(env, status, false);
  return MakeFirmwareVersion(env, version);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getPeriodicStatus0
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getPeriodicStatus0
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_A301PeriodicStatus0 frame{};
  HAL_GetA301PeriodicStatus0(handle, &frame, &status);
  return MakePeriodicStatus0(env, frame, status);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getPeriodicStatus1
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getPeriodicStatus1
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_A301PeriodicStatus1 frame{};
  HAL_GetA301PeriodicStatus1(handle, &frame, &status);
  return MakePeriodicStatus1(env, frame, status);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getPeriodicStatus2
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getPeriodicStatus2
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_A301PeriodicStatus2 frame{};
  HAL_GetA301PeriodicStatus2(handle, &frame, &status);
  return MakePeriodicStatus2(env, frame, status);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getPeriodicStatus3
 * Signature: (I)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getPeriodicStatus3
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_A301PeriodicStatus3 frame{};
  HAL_GetA301PeriodicStatus3(handle, &frame, &status);
  return MakePeriodicStatus3(env, frame, status);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setRelativeEncoderPosition
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setRelativeEncoderPosition
  (JNIEnv* env, jclass, jint handle, jdouble position)
{
  int32_t status = 0;
  HAL_SetA301RelativeEncoderPosition(handle, position, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setAbsoluteEncoderPosition
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setAbsoluteEncoderPosition
  (JNIEnv* env, jclass, jint handle, jdouble position)
{
  int32_t status = 0;
  HAL_SetA301AbsoluteEncoderPosition(handle, position, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setSetpoint
 * Signature: (IDID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setSetpoint
  (JNIEnv* env, jclass, jint handle, jdouble value, jint controlType,
   jdouble positionSpeed)
{
  int32_t status = 0;
  HAL_SetA301Setpoint(handle, value,
                      static_cast<HAL_A301ControlType>(controlType),
                      positionSpeed, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setIdleMode
 * Signature: (II)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setIdleMode
  (JNIEnv* env, jclass, jint handle, jint idleMode)
{
  int32_t status = 0;
  HAL_SetA301IdleMode(handle, static_cast<HAL_A301IdleMode>(idleMode), &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getIdleMode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getIdleMode
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetA301IdleMode(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setAbsolutePositionContinuousInput
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setAbsolutePositionContinuousInput
  (JNIEnv* env, jclass, jint handle, jboolean enabled)
{
  int32_t status = 0;
  HAL_SetA301AbsolutePositionContinuousInput(handle, enabled, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getAbsolutePositionContinuousInput
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getAbsolutePositionContinuousInput
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetA301AbsolutePositionContinuousInput(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setAbsoluteEncoderRangeOffset
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setAbsoluteEncoderRangeOffset
  (JNIEnv* env, jclass, jint handle, jdouble offset)
{
  int32_t status = 0;
  HAL_SetA301AbsoluteEncoderRangeOffset(handle, offset, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getAbsoluteEncoderRangeOffset
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getAbsoluteEncoderRangeOffset
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetA301AbsoluteEncoderRangeOffset(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setInverted
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setInverted
  (JNIEnv* env, jclass, jint handle, jboolean inverted)
{
  int32_t status = 0;
  HAL_SetA301Inverted(handle, inverted, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getInverted
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getInverted
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  auto result = HAL_GetA301Inverted(handle, &status);
  CheckStatus(env, status, false);
  return result;
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    clearFaults
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_clearFaults
  (JNIEnv* env, jclass, jint handle)
{
  int32_t status = 0;
  HAL_ClearA301Faults(handle, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    setStatusFramePeriod
 * Signature: (III)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_A301JNI_setStatusFramePeriod
  (JNIEnv* env, jclass, jint handle, jint frame, jint periodMs)
{
  int32_t status = 0;
  HAL_SetA301StatusFramePeriod(
      handle, static_cast<HAL_A301PeriodicFrame>(frame), periodMs, &status);
  CheckStatus(env, status, false);
}

/*
 * Class:     org_wpilib_hardware_hal_A301JNI
 * Method:    getStatusFramePeriod
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL
Java_org_wpilib_hardware_hal_A301JNI_getStatusFramePeriod
  (JNIEnv* env, jclass, jint handle, jint frame)
{
  int32_t status = 0;
  int32_t result = HAL_GetA301StatusFramePeriod(
      handle, static_cast<HAL_A301PeriodicFrame>(frame), &status);
  CheckStatus(env, status, false);
  return result;
}

}  // extern "C"
