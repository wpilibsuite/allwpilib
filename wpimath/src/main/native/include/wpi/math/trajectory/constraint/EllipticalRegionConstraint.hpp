// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <limits>

#include "frc/geometry/Ellipse2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "units/length.h"

namespace frc {

/**
 * Enforces a particular constraint only within an elliptical region.
 */
template <std::derived_from<TrajectoryConstraint> Constraint>
class EllipticalRegionConstraint : public TrajectoryConstraint {
 public:
  /**
   * Constructs a new EllipticalRegionConstraint.
   *
   * @param center The center of the ellipse in which to enforce the constraint.
   * @param xWidth The width of the ellipse in which to enforce the constraint.
   * @param yWidth The height of the ellipse in which to enforce the constraint.
   * @param rotation The rotation to apply to all radii around the origin.
   * @param constraint The constraint to enforce when the robot is within the
   *     region.
   * @deprecated Use constructor taking Ellipse2d instead.
   */
  [[deprecated("Use constructor taking Ellipse2d instead.")]]
  constexpr EllipticalRegionConstraint(const Translation2d& center,
                                       units::meter_t xWidth,
                                       units::meter_t yWidth,
                                       const Rotation2d& rotation,
                                       const Constraint& constraint)
      : m_ellipse{Pose2d{center, rotation}, xWidth / 2.0, yWidth / 2.0},
        m_constraint(constraint) {}

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

  constexpr units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override {
    if (m_ellipse.Contains(pose.Translation())) {
      return m_constraint.MaxVelocity(pose, curvature, velocity);
    } else {
      return units::meters_per_second_t{
          std::numeric_limits<double>::infinity()};
    }
  }

  constexpr MinMax MinMaxAcceleration(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t speed) const override {
    if (m_ellipse.Contains(pose.Translation())) {
      return m_constraint.MinMaxAcceleration(pose, curvature, speed);
    } else {
      return {};
    }
  }

 private:
  Ellipse2d m_ellipse;
  Constraint m_constraint;
};

}  // namespace frc
