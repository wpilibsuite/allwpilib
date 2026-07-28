// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/IMU.h"

#include <cstring>
#include <numbers>

#include "HALInitializer.hpp"
#include "mrclib/IMU.h"
#include "wpi/hal/Errors.h"

namespace {

struct IMUInitializer {
  MRC_Status status = MRC_IMU_Initialize();
};

static MRC_Status InitMrcImu() {
  static IMUInitializer init;
  return init.status;
}

constexpr double kDegreesToRadians = std::numbers::pi / 180.0;
}  // namespace

namespace wpi::hal::init {
void InitializeIMU() {}
}  // namespace wpi::hal::init

extern "C" {
void HAL_GetIMUAcceleration(HAL_Acceleration3d* accel, int32_t* status) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *status = initStatus;
    return;
  }

  MRC_Acceleration3d accelData;
  std::memset(&accelData, 0, sizeof(accelData));
  *status = MRC_IMU_GetAcceleration(&accelData);
  *accel = HAL_Acceleration3d{.timestamp = accelData.timestamp,
                              .x = accelData.x,
                              .y = accelData.y,
                              .z = accelData.z};
}

void HAL_GetIMUGyroRates(HAL_GyroRate3d* rate, int32_t* status) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *status = initStatus;
    return;
  }

  MRC_GyroRate3d rateData;
  std::memset(&rateData, 0, sizeof(rateData));
  *status = MRC_IMU_GetGyroRate(&rateData);
  *rate = HAL_GyroRate3d{.timestamp = rateData.timestamp,
                         .x = rateData.x,
                         .y = rateData.y,
                         .z = rateData.z};
}

void HAL_GetIMUEulerAnglesFlat(HAL_EulerAngles3d* angles, int32_t* status) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *status = initStatus;
    return;
  }

  MRC_EulerAngles3d angleData;
  std::memset(&angleData, 0, sizeof(angleData));
  *status = MRC_IMU_GetEulerAnglesFlat(&angleData);
  *angles = HAL_EulerAngles3d{.timestamp = angleData.timestamp,
                              .x = angleData.x,
                              .y = angleData.y,
                              .z = angleData.z};
}

void HAL_GetIMUEulerAnglesLandscape(HAL_EulerAngles3d* angles,
                                    int32_t* status) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *status = initStatus;
    return;
  }

  MRC_EulerAngles3d angleData;
  std::memset(&angleData, 0, sizeof(angleData));
  *status = MRC_IMU_GetEulerAnglesLandscape(&angleData);
  *angles = HAL_EulerAngles3d{.timestamp = angleData.timestamp,
                              .x = angleData.x,
                              .y = angleData.y,
                              .z = angleData.z};
}

void HAL_GetIMUEulerAnglesPortrait(HAL_EulerAngles3d* angles, int32_t* status) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *status = initStatus;
    return;
  }

  MRC_EulerAngles3d angleData;
  std::memset(&angleData, 0, sizeof(angleData));
  *status = MRC_IMU_GetEulerAnglesPortrait(&angleData);
  *angles = HAL_EulerAngles3d{.timestamp = angleData.timestamp,
                              .x = angleData.x,
                              .y = angleData.y,
                              .z = angleData.z};
}

void HAL_GetIMUQuaternion(HAL_Quaternion* quat, int32_t* status) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *status = initStatus;
    return;
  }

  MRC_Quaternion quatData;
  std::memset(&quatData, 0, sizeof(quatData));
  *status = MRC_IMU_GetQuaternion(&quatData);
  *quat = HAL_Quaternion{.timestamp = quatData.timestamp,
                         .w = quatData.w,
                         .x = quatData.x,
                         .y = quatData.y,
                         .z = quatData.z};
}

double HAL_GetIMUYawFlat(int64_t* timestamp) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *timestamp = 0;
    return 0;
  }

  double ret = 0;
  *timestamp = 0;
  MRC_IMU_GetYawFlat(&ret, timestamp);
  return ret;
}

double HAL_GetIMUYawLandscape(int64_t* timestamp) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *timestamp = 0;
    return 0;
  }

  double ret = 0;
  *timestamp = 0;
  MRC_IMU_GetYawLandscape(&ret, timestamp);
  return ret;
}

double HAL_GetIMUYawPortrait(int64_t* timestamp) {
  MRC_Status initStatus = InitMrcImu();
  if (initStatus != MRC_STATUS_SUCCESS) {
    *timestamp = 0;
    return 0;
  }

  double ret = 0;
  *timestamp = 0;
  MRC_IMU_GetYawPortrait(&ret, timestamp);
  return ret;
}
}  // extern "C"
