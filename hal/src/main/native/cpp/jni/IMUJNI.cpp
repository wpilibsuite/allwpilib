// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <jni.h>

#include <string_view>

#include <fmt/format.h>

#include "HALUtil.h"
#include "org_wpilib_hardware_hal_IMUJNI.h"
#include "hal/IMU.h"

using namespace hal;

namespace {
void assertArraySize(JNIEnv* env, jarray array, int minimumSize,
                     std::string_view arrayName) {
  jsize actualSize = env->GetArrayLength(array);
  if (actualSize < minimumSize) {
    ThrowIllegalArgumentException(
        env, fmt::format("{} array too small: expected at least {}, got {}",
                         arrayName, minimumSize, actualSize));
  }
}
}  // namespace

extern "C" {

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUAcceleration
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUAcceleration
  (JNIEnv* env, jclass, jdoubleArray accel)
{
  assertArraySize(env, accel, 3, "accel");

  int32_t status = 0;
  HAL_Acceleration3d data;

  HAL_GetIMUAcceleration(&data, &status);
  CheckStatus(env, status);

  double arr[]{data.x, data.y, data.z};
  env->SetDoubleArrayRegion(accel, 0, 3, arr);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUGyroRates
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUGyroRates
  (JNIEnv* env, jclass, jdoubleArray rates)
{
  assertArraySize(env, rates, 3, "rates");

  int32_t status = 0;
  HAL_GyroRate3d data;

  HAL_GetIMUGyroRates(&data, &status);
  CheckStatus(env, status);

  double arr[]{data.x, data.y, data.z};
  env->SetDoubleArrayRegion(rates, 0, 3, arr);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUEulerAnglesFlat
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUEulerAnglesFlat
  (JNIEnv* env, jclass, jdoubleArray angles)
{
  assertArraySize(env, angles, 3, "angles");

  int32_t status = 0;
  HAL_EulerAngles3d data;

  HAL_GetIMUEulerAnglesFlat(&data, &status);
  CheckStatus(env, status);

  double arr[]{data.x, data.y, data.z};
  env->SetDoubleArrayRegion(angles, 0, 3, arr);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUEulerAnglesLandscape
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUEulerAnglesLandscape
  (JNIEnv* env, jclass, jdoubleArray angles)
{
  assertArraySize(env, angles, 3, "angles");

  int32_t status = 0;
  HAL_EulerAngles3d data;

  HAL_GetIMUEulerAnglesLandscape(&data, &status);
  CheckStatus(env, status);

  double arr[]{data.x, data.y, data.z};
  env->SetDoubleArrayRegion(angles, 0, 3, arr);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUEulerAnglesPortrait
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUEulerAnglesPortrait
  (JNIEnv* env, jclass, jdoubleArray angles)
{
  assertArraySize(env, angles, 3, "angles");

  int32_t status = 0;
  HAL_EulerAngles3d data;

  HAL_GetIMUEulerAnglesPortrait(&data, &status);
  CheckStatus(env, status);

  double arr[]{data.x, data.y, data.z};
  env->SetDoubleArrayRegion(angles, 0, 3, arr);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUQuaternion
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUQuaternion
  (JNIEnv* env, jclass, jdoubleArray quat)
{
  assertArraySize(env, quat, 4, "quat");

  int32_t status = 0;
  HAL_Quaternion data;

  HAL_GetIMUQuaternion(&data, &status);
  CheckStatus(env, status);

  double arr[]{data.w, data.x, data.y, data.z};
  env->SetDoubleArrayRegion(quat, 0, 4, arr);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUYawFlat
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUYawFlat
  (JNIEnv* env, jclass)
{
  int64_t timestamp;
  return HAL_GetIMUYawFlat(&timestamp);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUYawLandscape
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUYawLandscape
  (JNIEnv* env, jclass)
{
  int64_t timestamp;
  return HAL_GetIMUYawLandscape(&timestamp);
}

/*
 * Class:     org_wpilib_hardware_hal_IMUJNI
 * Method:    getIMUYawPortrait
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_org_wpilib_hardware_hal_IMUJNI_getIMUYawPortrait
  (JNIEnv* env, jclass)
{
  int64_t timestamp;
  return HAL_GetIMUYawPortrait(&timestamp);
}

}  // extern "C"
