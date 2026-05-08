// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "IMUDataInternal.hpp"

using namespace wpi::hal;

namespace wpi::hal {
namespace init {
void InitializeIMUData() {
  static IMUData imu;
  ::wpi::hal::SimIMUData = &imu;
}
}  // namespace init

IMUData* SimIMUData;
}  // namespace wpi::hal

extern "C" {
#define DEFINE_CAPI(TYPE, CAPINAME, LOWERNAME)                      \
  HAL_SIMDATAVALUE_DEFINE_CAPI_NOINDEX(TYPE, HALSIM, IMU##CAPINAME, \
                                       SimIMUData, LOWERNAME)

DEFINE_CAPI(double, AngleX, angleX)
DEFINE_CAPI(double, AngleY, angleY)
DEFINE_CAPI(double, AngleZ, angleZ)

DEFINE_CAPI(double, GyroRateX, gyroRateX)
DEFINE_CAPI(double, GyroRateY, gyroRateY)
DEFINE_CAPI(double, GyroRateZ, gyroRateZ)

DEFINE_CAPI(double, AccelX, accelX)
DEFINE_CAPI(double, AccelY, accelY)
DEFINE_CAPI(double, AccelZ, accelZ)

DEFINE_CAPI(double, Yaw, yaw)

}  // extern "C"
