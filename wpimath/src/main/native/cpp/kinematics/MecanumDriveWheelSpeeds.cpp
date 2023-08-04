// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/MecanumDriveWheelSpeeds.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "units/math.h"

using namespace frc;

void MecanumDriveWheelSpeeds::Desaturate(
    units::meters_per_second_t attainableMaxSpeed) {
  std::array<units::meters_per_second_t, 4> wheelSpeeds{frontLeft, frontRight,
                                                        rearLeft, rearRight};
  units::meters_per_second_t realMaxSpeed = *std::max_element(
      wheelSpeeds.begin(), wheelSpeeds.end(), [](const auto& a, const auto& b) {
        return units::math::abs(a) < units::math::abs(b);
      });

  if (realMaxSpeed > attainableMaxSpeed) {
    for (int i = 0; i < 4; ++i) {
      wheelSpeeds[i] = wheelSpeeds[i] / realMaxSpeed * attainableMaxSpeed;
    }
    frontLeft = wheelSpeeds[0];
    frontRight = wheelSpeeds[1];
    rearLeft = wheelSpeeds[2];
    rearRight = wheelSpeeds[3];
  }
}
