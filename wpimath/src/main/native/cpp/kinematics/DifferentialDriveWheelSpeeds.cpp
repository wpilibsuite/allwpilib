// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"

#include "units/math.h"

using namespace frc;

void DifferentialDriveWheelSpeeds::Desaturate(
    units::meters_per_second_t attainableMaxSpeed) {
  auto realMaxSpeed =
      units::math::max(units::math::abs(left), units::math::abs(right));

  if (realMaxSpeed > attainableMaxSpeed) {
    left = left / realMaxSpeed * attainableMaxSpeed;
    right = right / realMaxSpeed * attainableMaxSpeed;
  }
}
