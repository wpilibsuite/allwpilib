// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/HolonomicTrajectory.hpp"

#include <utility>
#include <vector>

#include "wpi/math/trajectory/TrajectorySample.hpp"

using namespace wpi::math;

TrajectorySample HolonomicTrajectory::Interpolate(const TrajectorySample& start,
                                                  const TrajectorySample& end,
                                                  double t) const {
  return KinematicInterpolate(start, end, t);
}
