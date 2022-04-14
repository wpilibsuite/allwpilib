// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "units/length.h"

namespace frc {
/**
 * Enforces a particular constraint only within an elliptical region.
 */
template <typename Constraint, typename = std::enable_if_t<std::is_base_of_v<
                                   TrajectoryConstraint, Constraint>>>
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
   * region.
   */
  EllipticalRegionConstraint(const Translation2d& center, units::meter_t xWidth,
                             units::meter_t yWidth, const Rotation2d& rotation,
                             const Constraint& constraint)
      : m_center(center),
        m_radii(xWidth / 2.0, yWidth / 2.0),
        m_constraint(constraint) {
    m_radii = m_radii.RotateBy(rotation);
  }

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override {
    if (IsPoseInRegion(pose)) {
      return m_constraint.MaxVelocity(pose, curvature, velocity);
    } else {
      return units::meters_per_second_t(
          std::numeric_limits<double>::infinity());
    }
  }

  MinMax MinMaxAcceleration(const Pose2d& pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) const override {
    if (IsPoseInRegion(pose)) {
      return m_constraint.MinMaxAcceleration(pose, curvature, speed);
    } else {
      return {};
    }
  }

  /**
   * Returns whether the specified robot pose is within the region that the
   * constraint is enforced in.
   *
   * @param pose The robot pose.
   * @return Whether the robot pose is within the constraint region.
   */
  bool IsPoseInRegion(const Pose2d& pose) const {
    // The region (disk) bounded by the ellipse is given by the equation:
    // ((x-h)^2)/Rx^2) + ((y-k)^2)/Ry^2) <= 1
    // If the inequality is satisfied, then it is inside the ellipse; otherwise
    // it is outside the ellipse.
    // Both sides have been multiplied by Rx^2 * Ry^2 for efficiency reasons.
    return units::math::pow<2>(pose.X() - m_center.X()) *
                   units::math::pow<2>(m_radii.Y()) +
               units::math::pow<2>(pose.Y() - m_center.Y()) *
                   units::math::pow<2>(m_radii.X()) <=
           units::math::pow<2>(m_radii.X()) * units::math::pow<2>(m_radii.Y());
  }

 private:
  Translation2d m_center;
  Translation2d m_radii;
  Constraint m_constraint;
};
}  // namespace frc
