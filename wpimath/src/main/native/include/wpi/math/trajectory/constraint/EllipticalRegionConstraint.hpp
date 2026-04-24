// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <limits>

#include "wpi/math/geometry/Ellipse2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/trajectory/constraint/TrajectoryConstraint.hpp"
#include "wpi/units/length.hpp"

namespace wpi::math {

/**
 * Enforces a particular constraint only within an elliptical region.
 */
template <std::derived_from<TrajectoryConstraint> Constraint>
class EllipticalRegionConstraint : public TrajectoryConstraint {
 public:
  /**
   * Constructs a new EllipticalRegionConstraint.
   *
   * @param ellipse The ellipse in which to enforce the constraint.
   * @param constraint The constraint to enforce when the robot is within the
   *     region.
   */
  constexpr EllipticalRegionConstraint(const Ellipse2d& ellipse,
                                       const Constraint& constraint)
      : m_ellipse{ellipse}, m_constraint{constraint} {}

  constexpr wpi::units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, wpi::units::curvature_t curvature,
      wpi::units::meters_per_second_t velocity) const override {
    if (m_ellipse.Contains(pose.Translation())) {
      return m_constraint.MaxVelocity(pose, curvature, velocity);
    } else {
      return wpi::units::meters_per_second_t{
          std::numeric_limits<double>::infinity()};
    }
  }

  constexpr MinMax MinMaxAcceleration(
      const Pose2d& pose, wpi::units::curvature_t curvature,
      wpi::units::meters_per_second_t velocity) const override {
    if (m_ellipse.Contains(pose.Translation())) {
      return m_constraint.MinMaxAcceleration(pose, curvature, velocity);
    } else {
      return {};
    }
  }

 private:
  Ellipse2d m_ellipse;
  Constraint m_constraint;
};

}  // namespace wpi::math
