// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/IMU.h"

#include <networktables/DoubleArrayTopic.h>
#include <networktables/DoubleTopic.h>
#include <networktables/NetworkTableInstance.h>

#include "HALInitializer.h"
#include "SystemServerInternal.h"
#include "hal/Errors.h"

#define IMU_PREFIX "imu/"

namespace {

constexpr const char* kRawAccelKey = IMU_PREFIX "rawaccel";
constexpr const char* kRawGyroKey = IMU_PREFIX "rawgyro";
constexpr const char* kQuaternionKey = IMU_PREFIX "quat";
constexpr const char* kEulerAngleKey = IMU_PREFIX "euler";
constexpr const char* kYawFlatKey = IMU_PREFIX "yaw_flat";
constexpr const char* kYawLandscapeKey = IMU_PREFIX "yaw_landscape";
constexpr const char* kYawPortraitKey = IMU_PREFIX "yaw_portrait";

struct IMU {
  explicit IMU(nt::NetworkTableInstance inst)
      : rawAccelSub{inst.GetDoubleArrayTopic(kRawAccelKey).Subscribe({})},
        rawGyroSub{inst.GetDoubleArrayTopic(kRawGyroKey).Subscribe({})},
        quatSub{inst.GetDoubleArrayTopic(kQuaternionKey).Subscribe({})},
        eulerAngleSub{inst.GetDoubleArrayTopic(kEulerAngleKey).Subscribe({})},
        yawFlatSub{inst.GetDoubleTopic(kYawFlatKey).Subscribe(0)},
        yawLandscapeSub{inst.GetDoubleTopic(kYawLandscapeKey).Subscribe(0)},
        yawPortraitSub{inst.GetDoubleTopic(kYawPortraitKey).Subscribe(0)} {}

  nt::DoubleArraySubscriber rawAccelSub;
  nt::DoubleArraySubscriber rawGyroSub;
  nt::DoubleArraySubscriber quatSub;
  nt::DoubleArraySubscriber eulerAngleSub;
  nt::DoubleSubscriber yawFlatSub;
  nt::DoubleSubscriber yawLandscapeSub;
  nt::DoubleSubscriber yawPortraitSub;
};

static IMU* imu;

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
  *accel = HAL_Acceleration3d{.timestamp = update.time,
                              .x = update.value[0],
                              .y = update.value[1],
                              .z = update.value[2]};
}

void HAL_GetIMUGyroRates(HAL_GyroRate3d* rate, int32_t* status) {
  auto update = imu->rawGyroSub.GetAtomic();
  if (update.value.size() != 3) {
    *status = INCOMPATIBLE_STATE;
    return;
  }

  *rate = HAL_GyroRate3d{.timestamp = update.time,
                         .x = update.value[0],
                         .y = update.value[1],
                         .z = update.value[2]};
}

void HAL_GetIMUEulerAngles(HAL_EulerAngles3d* angles, int32_t* status) {
  auto update = imu->eulerAngleSub.GetAtomic();
  if (update.value.size() != 3) {
    *status = INCOMPATIBLE_STATE;
    return;
  }
  *angles = HAL_EulerAngles3d{.timestamp = update.time,
                              .x = update.value[0],
                              .y = update.value[1],
                              .z = update.value[2]};
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
  return update.value;
}

double HAL_GetIMUYawLandscape(int64_t* timestamp) {
  auto update = imu->yawLandscapeSub.GetAtomic();
  *timestamp = update.time;
  return update.value;
}

double HAL_GetIMUYawPortrait(int64_t* timestamp) {
  auto update = imu->yawFlatSub.GetAtomic();
  *timestamp = update.time;
  return update.value;
}
}  // extern "C"
