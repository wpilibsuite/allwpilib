// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/simulation/SimDataValue.hpp"

extern "C" {
#define DEFINE_CAPI(TYPE, CAPINAME, RETURN) \
  HAL_SIMDATAVALUE_STUB_CAPI(TYPE, HALSIM, IMU##CAPINAME, RETURN)

DEFINE_CAPI(double, AngleX, 0)
DEFINE_CAPI(double, AngleY, 0)
DEFINE_CAPI(double, AngleZ, 0)

DEFINE_CAPI(double, GyroRateX, 0)
DEFINE_CAPI(double, GyroRateY, 0)
DEFINE_CAPI(double, GyroRateZ, 0)

DEFINE_CAPI(double, AccelX, 0)
DEFINE_CAPI(double, AccelY, 0)
DEFINE_CAPI(double, AccelZ, 0)

DEFINE_CAPI(double, Yaw, 0)

}  // extern "C"
