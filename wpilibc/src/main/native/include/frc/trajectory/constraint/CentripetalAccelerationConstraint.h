/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {

/**
 * A constraint on the maximum absolute centripetal acceleration allowed when
 * traversing a trajectory. The centripetal acceleration of a robot is defined
 * as the velocity squared divided by the radius of curvature.
 *
 * Effectively, limiting the maximum centripetal acceleration will cause the
 * robot to slow down around tight turns, making it easier to track trajectories
 * with sharp turns.
 */
class CentripetalAccelerationConstraint : public TrajectoryConstraint {
 public:
  explicit CentripetalAccelerationConstraint(
      units::meters_per_second_squared_t maxCentripetalAcceleration);

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, curvature_t curvature,
      units::meters_per_second_t velocity) override;

  MinMax MinMaxAcceleration(const Pose2d& pose, curvature_t curvature,
                            units::meters_per_second_t speed) override;

 private:
  units::meters_per_second_squared_t m_maxCentripetalAcceleration;
};
}  // namespace frc
