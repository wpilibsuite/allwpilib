// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include "frc/geometry/Pose2d.h"
#include "frc/spline/Spline.h"
#include "units/acceleration.h"
#include "units/curvature.h"
#include "units/velocity.h"

namespace frc {
/**
 * An interface for defining user-defined velocity and acceleration constraints
 * while generating trajectories.
 */
class TrajectoryConstraint {
 public:
  TrajectoryConstraint() = default;

  TrajectoryConstraint(const TrajectoryConstraint&) = default;
  TrajectoryConstraint& operator=(const TrajectoryConstraint&) = default;

  TrajectoryConstraint(TrajectoryConstraint&&) = default;
  TrajectoryConstraint& operator=(TrajectoryConstraint&&) = default;

  virtual ~TrajectoryConstraint() = default;

  /**
   * Represents a minimum and maximum acceleration.
   */
  struct MinMax {
    /**
     * The minimum acceleration.
     */
    units::meters_per_second_squared_t minAcceleration{
        -std::numeric_limits<double>::max()};

    /**
     * The maximum acceleration.
     */
    units::meters_per_second_squared_t maxAcceleration{
        std::numeric_limits<double>::max()};
  };

  /**
   * Returns the max velocity given the current pose and curvature.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory.
   * @param velocity The velocity at the current point in the trajectory before
   *                                constraints are applied.
   *
   * @return The absolute maximum velocity.
   */
  virtual units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const = 0;

  /**
   * Returns the minimum and maximum allowable acceleration for the trajectory
   * given pose, curvature, and speed.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory.
   * @param speed The speed at the current point in the trajectory.
   *
   * @return The min and max acceleration bounds.
   */
  virtual MinMax MinMaxAcceleration(const Pose2d& pose,
                                    units::curvature_t curvature,
                                    units::meters_per_second_t speed) const = 0;
};
}  // namespace frc
