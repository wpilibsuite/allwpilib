// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <random>

#include <wpi/array.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Twist2d.h"

namespace frc {

template <std::uniform_random_bit_generator Generator>
frc::Rotation2d SampleRotation2d(Generator& generator,
                                 std::normal_distribution<double>& distribution,
                                 double stdDev) {
  return frc::Rotation2d{units::radian_t{distribution(generator) * stdDev}};
}

template <std::uniform_random_bit_generator Generator>
frc::Twist2d SampleTwist2d(Generator& generator,
                           std::normal_distribution<double>& distribution,
                           wpi::array<double, 3> stdDev) {
  return frc::Twist2d{units::meter_t{distribution(generator) * stdDev[0]},
                      units::meter_t{distribution(generator) * stdDev[1]},
                      units::radian_t{distribution(generator) * stdDev[2]}};
}

}  // namespace frc
