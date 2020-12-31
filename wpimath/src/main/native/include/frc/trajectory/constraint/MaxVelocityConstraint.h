// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "units/math.h"
#include "units/velocity.h"

namespace frc {
/**
 * Represents a constraint that enforces a max velocity. This can be composed
 * with the EllipticalRegionConstraint or RectangularRegionConstraint to enforce
 * a max velocity within a region.
 */
class MaxVelocityConstraint : public TrajectoryConstraint {
 public:
  /**
   * Constructs a new MaxVelocityConstraint.
   *
   * @param maxVelocity The max velocity.
   */
  explicit MaxVelocityConstraint(units::meters_per_second_t maxVelocity)
      : m_maxVelocity(units::math::abs(maxVelocity)) {}

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override {
    return m_maxVelocity;
  }

  MinMax MinMaxAcceleration(const Pose2d& pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) const override {
    return {};
  }

 private:
  units::meters_per_second_t m_maxVelocity;
};
}  // namespace frc
