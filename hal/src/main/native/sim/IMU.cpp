// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/IMU.h"

extern "C" {
// TODO(Ryan) implement sim
void HAL_GetIMUAcceleration(HAL_Acceleration3d* accel, int32_t* status) {
  *accel = {};
}
void HAL_GetIMUGyroRates(HAL_GyroRate3d* rate, int32_t* status) {
  *rate = {};
}
void HAL_GetIMUEulerAngles(HAL_EulerAngles3d* angles, int32_t* status) {
  *angles = {};
}
void HAL_GetIMUQuaternion(HAL_Quaternion* quat, int32_t* status) {
  *quat = {};
}
double HAL_GetIMUYawFlat(int64_t* timestamp) {
  return 0;
}
double HAL_GetIMUYawLandscape(int64_t* timestamp) {
  return 0;
}
double HAL_GetIMUYawPortrait(int64_t* timestamp) {
  return 0;
}
}  // extern "C"
