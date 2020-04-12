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
 * Enforces a particular constraint only within a rectangular region.
 */
class RectangularRegionConstraint : public TrajectoryConstraint {
 public:
  /**
   * Constructs a new RectangularRegionConstraint.
   *
   * @param bottomLeftPoint The bottom left point of the rectangular region in
   * which to enforce the constraint.
   * @param topRightPoint The top right point of the rectangular region in which
   * to enforce the constraint.
   * @param constraint The constraint to enforce when the robot is within the
   * region.
   */
  RectangularRegionConstraint(const Translation2d& bottomLeftPoint,
                              const Translation2d& topRightPoint,
                              TrajectoryConstraint& constraint);

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
  Translation2d m_bottomLeftPoint;
  Translation2d m_topRightPoint;
  TrajectoryConstraint& m_constraint;
};
}  // namespace frc
