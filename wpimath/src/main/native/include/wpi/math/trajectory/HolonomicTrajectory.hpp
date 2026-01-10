// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include "wpi/math/trajectory/Trajectory.hpp"

namespace wpi::math {
class WPILIB_DLLEXPORT HolonomicTrajectory
    : public Trajectory<TrajectorySample> {
 public:
  explicit HolonomicTrajectory(std::vector<TrajectorySample> samples)
      : Trajectory<TrajectorySample>(std::move(samples)) {}

  /**
   * Interpolates between two samples using kinematic interpolation.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  TrajectorySample Interpolate(const TrajectorySample& start,
                               const TrajectorySample& end,
                               double t) const final;

  /**
   * Transforms all poses in the trajectory by the given transform.
   *
   * @param transform The transform to apply to the poses.
   * @return The transformed trajectory.
   */
  HolonomicTrajectory TransformBy(const Transform2d& transform) const;

  /**
   * Transforms all poses so they are relative to the given pose.
   *
   * @param pose The pose to make the trajectory relative to.
   * @return The transformed trajectory.
   */
  HolonomicTrajectory RelativeTo(const Pose2d& pose) const;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  HolonomicTrajectory Concatenate(const HolonomicTrajectory& other) const;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  HolonomicTrajectory operator+(const HolonomicTrajectory& other) const {
    return Concatenate(other);
  }
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const HolonomicTrajectory& trajectory);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, HolonomicTrajectory& trajectory);

}  // namespace wpi::math

#include "wpi/math/trajectory/proto/HolonomicTrajectoryProto.hpp"
