// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/simulation/SimDataValue.hpp"

namespace wpi::hal {

class IMUData {
  HAL_SIMDATAVALUE_DEFINE_NAME(AngleX)
  HAL_SIMDATAVALUE_DEFINE_NAME(AngleY)
  HAL_SIMDATAVALUE_DEFINE_NAME(AngleZ)

  HAL_SIMDATAVALUE_DEFINE_NAME(GyroRateX)
  HAL_SIMDATAVALUE_DEFINE_NAME(GyroRateY)
  HAL_SIMDATAVALUE_DEFINE_NAME(GyroRateZ)

  HAL_SIMDATAVALUE_DEFINE_NAME(AccelX)
  HAL_SIMDATAVALUE_DEFINE_NAME(AccelY)
  HAL_SIMDATAVALUE_DEFINE_NAME(AccelZ)

  HAL_SIMDATAVALUE_DEFINE_NAME(Yaw)

 public:
  SimDataValue<double, HAL_MakeDouble, GetAngleXName> angleX{0};
  SimDataValue<double, HAL_MakeDouble, GetAngleYName> angleY{0};
  SimDataValue<double, HAL_MakeDouble, GetAngleZName> angleZ{0};

  SimDataValue<double, HAL_MakeDouble, GetGyroRateXName> gyroRateX{0};
  SimDataValue<double, HAL_MakeDouble, GetGyroRateYName> gyroRateY{0};
  SimDataValue<double, HAL_MakeDouble, GetGyroRateZName> gyroRateZ{0};

  SimDataValue<double, HAL_MakeDouble, GetAccelXName> accelX{0};
  SimDataValue<double, HAL_MakeDouble, GetAccelYName> accelY{0};
  SimDataValue<double, HAL_MakeDouble, GetAccelZName> accelZ{0};

  SimDataValue<double, HAL_MakeDouble, GetYawName> yaw{0};
};
extern IMUData* SimIMUData;

}  // namespace wpi::hal
