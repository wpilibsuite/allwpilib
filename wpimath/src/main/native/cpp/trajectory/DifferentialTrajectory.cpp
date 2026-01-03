// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DifferentialTrajectory.hpp"

#include <utility>
#include <vector>

#include "wpi/util/MathExtras.hpp"

using namespace wpi::math;

DifferentialSample DifferentialTrajectory::Interpolate(
    const DifferentialSample& start, const DifferentialSample& end,
    double t) const {
  // Use kinematic interpolation for base TrajectorySample fields
  auto baseSample = KinematicInterpolate(start, end, t);

  // Interpolate wheel speeds
  auto leftSpeed = wpi::util::Lerp(start.leftSpeed, end.leftSpeed, t);
  auto rightSpeed = wpi::util::Lerp(start.rightSpeed, end.rightSpeed, t);

  return DifferentialSample(baseSample.timestamp, baseSample.pose,
                            baseSample.velocity, baseSample.acceleration,
                            leftSpeed, rightSpeed);
}
