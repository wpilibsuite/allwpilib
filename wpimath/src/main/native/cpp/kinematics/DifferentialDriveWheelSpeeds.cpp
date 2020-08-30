/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"

#include "units/math.h"

using namespace frc;

void DifferentialDriveWheelSpeeds::Normalize(
    units::meters_per_second_t attainableMaxSpeed) {
  auto realMaxSpeed =
      units::math::max(units::math::abs(left), units::math::abs(right));

  if (realMaxSpeed > attainableMaxSpeed) {
    left = left / realMaxSpeed * attainableMaxSpeed;
    right = right / realMaxSpeed * attainableMaxSpeed;
  }
}
