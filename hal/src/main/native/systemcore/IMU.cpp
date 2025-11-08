// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/IMU.h"

#include <numbers>

#include "HALInitializer.h"
#include "SystemServerInternal.h"
#include "wpi/hal/Errors.h"
#include "wpi/nt/DoubleArrayTopic.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"

#define IMU_PREFIX "/imu/"

namespace {

constexpr const char* kRawAccelKey = IMU_PREFIX "rawaccel";
constexpr const char* kRawGyroKey = IMU_PREFIX "rawgyro";
constexpr const char* kQuaternionKey = IMU_PREFIX "quat";
constexpr const char* kEulerAnglesFlatKey = IMU_PREFIX "euler_flat";
constexpr const char* kEulerAnglesLandscapeKey = IMU_PREFIX "euler_landscape";
constexpr const char* kEulerAnglesPortraitKey = IMU_PREFIX "euler_portrait";
constexpr const char* kYawFlatKey = IMU_PREFIX "yaw_flat";
constexpr const char* kYawLandscapeKey = IMU_PREFIX "yaw_landscape";
constexpr const char* kYawPortraitKey = IMU_PREFIX "yaw_portrait";

struct IMU {
  explicit IMU(nt::NetworkTableInstance inst)
      : rawAccelSub{inst.GetDoubleArrayTopic(kRawAccelKey).Subscribe({})},
        rawGyroSub{inst.GetDoubleArrayTopic(kRawGyroKey).Subscribe({})},
        quatSub{inst.GetDoubleArrayTopic(kQuaternionKey).Subscribe({})},
        eulerFlatSub{
            inst.GetDoubleArrayTopic(kEulerAnglesFlatKey).Subscribe({})},
        eulerLandscapeSub{
            inst.GetDoubleArrayTopic(kEulerAnglesLandscapeKey).Subscribe({})},
        eulerPortraitSub{
            inst.GetDoubleArrayTopic(kEulerAnglesPortraitKey).Subscribe({})},
        yawFlatSub{inst.GetDoubleTopic(kYawFlatKey).Subscribe(0)},
        yawLandscapeSub{inst.GetDoubleTopic(kYawLandscapeKey).Subscribe(0)},
        yawPortraitSub{inst.GetDoubleTopic(kYawPortraitKey).Subscribe(0)} {}

  nt::DoubleArraySubscriber rawAccelSub;
  nt::DoubleArraySubscriber rawGyroSub;
  nt::DoubleArraySubscriber quatSub;
  nt::DoubleArraySubscriber eulerFlatSub;
  nt::DoubleArraySubscriber eulerLandscapeSub;
  nt::DoubleArraySubscriber eulerPortraitSub;
  nt::DoubleSubscriber yawFlatSub;
  nt::DoubleSubscriber yawLandscapeSub;
  nt::DoubleSubscriber yawPortraitSub;
};

static IMU* imu;

constexpr double kDegreesToRadians = std::numbers::pi / 180.0;
constexpr double kGToMetersPerSecondSquared = 9.80665;
}  // namespace

namespace hal::init {
void InitializeIMU() {
  static IMU imu_static{hal::GetSystemServer()};
  imu = &imu_static;
}
}  // namespace hal::init

extern "C" {
void HAL_GetIMUAcceleration(HAL_Acceleration3d* accel, int32_t* status) {
  auto update = imu->rawAccelSub.GetAtomic();
  if (update.value.size() != 3) {
    *status = INCOMPATIBLE_STATE;
    return;
  }
  *accel =
      HAL_Acceleration3d{.timestamp = update.time,
                         .x = update.value[0] * kGToMetersPerSecondSquared,
                         .y = update.value[1] * kGToMetersPerSecondSquared,
                         .z = update.value[2] * kGToMetersPerSecondSquared};
}

void HAL_GetIMUGyroRates(HAL_GyroRate3d* rate, int32_t* status) {
  auto update = imu->rawGyroSub.GetAtomic();
  if (update.value.size() != 3) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  *rate = HAL_GyroRate3d{.timestamp = update.time,
                         .x = update.value[0] * kDegreesToRadians,
                         .y = update.value[1] * kDegreesToRadians,
                         .z = update.value[2] * kDegreesToRadians};
}

void HAL_GetIMUEulerAnglesFlat(HAL_EulerAngles3d* angles, int32_t* status) {
  auto update = imu->eulerFlatSub.GetAtomic();
  if (update.value.size() != 3) {
    *status = INCOMPATIBLE_STATE;
    return;
  }
  *angles = HAL_EulerAngles3d{.timestamp = update.time,
                              .x = update.value[0] * kDegreesToRadians,
                              .y = update.value[1] * kDegreesToRadians,
                              .z = update.value[2] * kDegreesToRadians};
}

void HAL_GetIMUEulerAnglesLandscape(HAL_EulerAngles3d* angles,
                                    int32_t* status) {
  auto update = imu->eulerLandscapeSub.GetAtomic();
  if (update.value.size() != 3) {
    *status = INCOMPATIBLE_STATE;
    return;
  }
  *angles = HAL_EulerAngles3d{.timestamp = update.time,
                              .x = update.value[0] * kDegreesToRadians,
                              .y = update.value[1] * kDegreesToRadians,
                              .z = update.value[2] * kDegreesToRadians};
}

void HAL_GetIMUEulerAnglesPortrait(HAL_EulerAngles3d* angles, int32_t* status) {
  auto update = imu->eulerPortraitSub.GetAtomic();
  if (update.value.size() != 3) {
    *status = INCOMPATIBLE_STATE;
    return;
  }
  *angles = HAL_EulerAngles3d{.timestamp = update.time,
                              .x = update.value[0] * kDegreesToRadians,
                              .y = update.value[1] * kDegreesToRadians,
                              .z = update.value[2] * kDegreesToRadians};
}

void HAL_GetIMUQuaternion(HAL_Quaternion* quat, int32_t* status) {
  auto update = imu->quatSub.GetAtomic();
  if (update.value.size() != 4) {
    *status = INCOMPATIBLE_STATE;
    return;
  }
  *quat = HAL_Quaternion{.timestamp = update.time,
                         .w = update.value[0],
                         .x = update.value[1],
                         .y = update.value[2],
                         .z = update.value[3]};
}

double HAL_GetIMUYawFlat(int64_t* timestamp) {
  auto update = imu->yawFlatSub.GetAtomic();
  *timestamp = update.time;
  return update.value * kDegreesToRadians;
}

double HAL_GetIMUYawLandscape(int64_t* timestamp) {
  auto update = imu->yawLandscapeSub.GetAtomic();
  *timestamp = update.time;
  return update.value * kDegreesToRadians;
}

double HAL_GetIMUYawPortrait(int64_t* timestamp) {
  auto update = imu->yawPortraitSub.GetAtomic();
  *timestamp = update.time;
  return update.value * kDegreesToRadians;
}
}  // extern "C"
