// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <limits>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/curvature.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * An interface for defining user-defined velocity and acceleration constraints
 * while generating trajectories.
 */
class WPILIB_DLLEXPORT TrajectoryConstraint {
 public:
  constexpr TrajectoryConstraint() = default;

  constexpr TrajectoryConstraint(const TrajectoryConstraint&) = default;
  constexpr TrajectoryConstraint& operator=(const TrajectoryConstraint&) =
      default;

  constexpr TrajectoryConstraint(TrajectoryConstraint&&) = default;
  constexpr TrajectoryConstraint& operator=(TrajectoryConstraint&&) = default;

  constexpr virtual ~TrajectoryConstraint() = default;

  /**
   * Represents a minimum and maximum acceleration.
   */
  struct MinMax {
    /**
     * The minimum acceleration.
     */
    wpi::units::meters_per_second_squared_t minAcceleration{
        -std::numeric_limits<double>::max()};

    /**
     * The maximum acceleration.
     */
    wpi::units::meters_per_second_squared_t maxAcceleration{
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
  constexpr virtual wpi::units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, wpi::units::curvature_t curvature,
      wpi::units::meters_per_second_t velocity) const = 0;

  /**
   * Returns the minimum and maximum allowable acceleration for the trajectory
   * given pose, curvature, and velocity.
   *
   * @param pose The pose at the current point in the trajectory.
   * @param curvature The curvature at the current point in the trajectory.
   * @param velocity The velocity at the current point in the trajectory.
   *
   * @return The min and max acceleration bounds.
   */
  constexpr virtual MinMax MinMaxAcceleration(
      const Pose2d& pose, wpi::units::curvature_t curvature,
      wpi::units::meters_per_second_t velocity) const = 0;
};
}  // namespace wpi::math
