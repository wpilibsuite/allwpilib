// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/trajectory/DrivetrainSplineSample.hpp"
#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * A trajectory for spline-based path following.
 *
 * This trajectory uses constant-acceleration kinematic equations for
 * interpolation between samples.
 */
class WPILIB_DLLEXPORT DrivetrainSplineTrajectory
    : public Trajectory<DrivetrainSplineSample> {
 public:
  /**
   * Constructs a DrivetrainSplineTrajectory from a vector of samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   * @throws std::invalid_argument if the vector of samples is empty.
   */
  explicit DrivetrainSplineTrajectory(
      std::vector<DrivetrainSplineSample> samples)
      : Trajectory<DrivetrainSplineSample>(std::move(samples)) {}

  /**
   * Constructs a DrivetrainSplineTrajectory from an initializer list of
   * samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   */
  DrivetrainSplineTrajectory(
      std::initializer_list<DrivetrainSplineSample> samples)
      : Trajectory<DrivetrainSplineSample>(
            std::vector<DrivetrainSplineSample>(samples)) {}

  /**
   * Interpolates between two samples using constant-acceleration kinematic
   * equations.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  DrivetrainSplineSample Interpolate(const DrivetrainSplineSample& start,
                                     const DrivetrainSplineSample& end,
                                     double t) const final;

  /**
   * Transforms all poses in the trajectory by the given transform.
   *
   * @param transform The transform to apply to the poses.
   * @return The transformed trajectory.
   */
  DrivetrainSplineTrajectory TransformBy(const Transform2d& transform) const;

  /**
   * Transforms all poses so they are relative to the given pose.
   *
   * @param pose The pose to make the trajectory relative to.
   * @return The transformed trajectory.
   */
  DrivetrainSplineTrajectory RelativeTo(const Pose2d& pose) const;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  DrivetrainSplineTrajectory Concatenate(
      const DrivetrainSplineTrajectory& other) const;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  DrivetrainSplineTrajectory operator+(
      const DrivetrainSplineTrajectory& other) const {
    return Concatenate(other);
  }
};

}  // namespace wpi::math
