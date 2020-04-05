/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {
/**
 * Enforces a particular constraint only within an elliptical region.
 */
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
                             TrajectoryConstraint* constraint);

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) override;

  MinMax MinMaxAcceleration(const Pose2d& pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) override;

  /**
   * Returns whether the specified robot pose is within the region that the
   * constraint is enforced in.
   *
   * @param pose The robot pose.
   * @return Whether the robot pose is within the constraint region.
   */
  bool IsPoseInRegion(const Pose2d& pose);

 private:
  Translation2d m_center;
  Translation2d m_radii;
  TrajectoryConstraint* m_constraint;
};
}  // namespace frc
