// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/IMU.h"

#include "mockdata/IMUDataInternal.hpp"

using namespace wpi::hal;

extern "C" {
void HAL_GetIMUAcceleration(HAL_Acceleration3d* accel, int32_t* status) {
  *accel = {
      .timestamp = 0,
      .x = SimIMUData->accelX,
      .y = SimIMUData->accelY,
      .z = SimIMUData->accelZ,
  };
}
void HAL_GetIMUGyroRates(HAL_GyroRate3d* rate, int32_t* status) {
  *rate = {
      .timestamp = 0,
      .x = SimIMUData->gyroRateX,
      .y = SimIMUData->gyroRateY,
      .z = SimIMUData->gyroRateZ,
  };
}
void HAL_GetIMUEulerAnglesFlat(HAL_EulerAngles3d* angles, int32_t* status) {
  *angles = {
      .timestamp = 0,
      .x = SimIMUData->angleX,
      .y = SimIMUData->angleY,
      .z = SimIMUData->angleZ,
  };
}
void HAL_GetIMUEulerAnglesLandscape(HAL_EulerAngles3d* angles,
                                    int32_t* status) {
  *angles = {
      .timestamp = 0,
      .x = SimIMUData->angleX,
      .y = SimIMUData->angleY,
      .z = SimIMUData->angleZ,
  };
}
void HAL_GetIMUEulerAnglesPortrait(HAL_EulerAngles3d* angles, int32_t* status) {
  *angles = {
      .timestamp = 0,
      .x = SimIMUData->angleX,
      .y = SimIMUData->angleY,
      .z = SimIMUData->angleZ,
  };
}
void HAL_GetIMUQuaternion(HAL_Quaternion* quat, int32_t* status) {
  *quat = {};
}
double HAL_GetIMUYawFlat(int64_t* timestamp) {
  return SimIMUData->yaw;
}
double HAL_GetIMUYawLandscape(int64_t* timestamp) {
  return SimIMUData->yaw;
}
double HAL_GetIMUYawPortrait(int64_t* timestamp) {
  return SimIMUData->yaw;
}
}  // extern "C"
