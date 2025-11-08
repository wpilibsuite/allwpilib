// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/trajectory/constraint/TrajectoryConstraint.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace frc {

/**
 * Represents a constraint that enforces a max velocity. This can be composed
 * with the EllipticalRegionConstraint or RectangularRegionConstraint to enforce
 * a max velocity within a region.
 */
class WPILIB_DLLEXPORT MaxVelocityConstraint : public TrajectoryConstraint {
 public:
  /**
   * Constructs a new MaxVelocityConstraint.
   *
   * @param maxVelocity The max velocity.
   */
  constexpr explicit MaxVelocityConstraint(
      units::meters_per_second_t maxVelocity)
      : m_maxVelocity(units::math::abs(maxVelocity)) {}

  constexpr units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override {
    return m_maxVelocity;
  }

  constexpr MinMax MinMaxAcceleration(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t speed) const override {
    return {};
  }

 private:
  units::meters_per_second_t m_maxVelocity;
};

}  // namespace frc
