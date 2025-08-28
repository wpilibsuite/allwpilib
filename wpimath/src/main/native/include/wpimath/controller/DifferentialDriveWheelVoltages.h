// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/voltage.h"

namespace wpimath {

/**
 * Motor voltages for a differential drive.
 */
struct DifferentialDriveWheelVoltages {
  /// Left wheel voltage.
  units::volt_t left = 0_V;

  /// Right wheel voltage.
  units::volt_t right = 0_V;
};

}  // namespace wpimath

#include "wpimath/controller/proto/DifferentialDriveWheelVoltagesProto.h"
#include "wpimath/controller/struct/DifferentialDriveWheelVoltagesStruct.h"
