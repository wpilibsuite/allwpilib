// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/voltage.h"

namespace wpi::math {

/**
 * Motor voltages for a differential drive.
 */
struct DifferentialDriveWheelVoltages {
  /// Left wheel voltage.
  units::volt_t left = 0_V;

  /// Right wheel voltage.
  units::volt_t right = 0_V;
};

}  // namespace wpi::math

#include "wpi/math/controller/proto/DifferentialDriveWheelVoltagesProto.h"
#include "wpi/math/controller/struct/DifferentialDriveWheelVoltagesStruct.h"
