// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/trajectory/SplineSample.hpp"
#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/units/math.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * A trajectory for spline-based path following.
 *
 * This trajectory uses constant-acceleration kinematic equations for
 * interpolation between samples.
 */
class WPILIB_DLLEXPORT SplineTrajectory
    : public Trajectory<SplineSample, SplineTrajectory> {
 public:
  /**
   * Constructs a SplineTrajectory from a vector of samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   * @throws std::invalid_argument if the vector of samples is empty.
   */
  explicit SplineTrajectory(std::vector<SplineSample> samples)
      : Trajectory<SplineSample, SplineTrajectory>(std::move(samples)) {}

  /**
   * Constructs a SplineTrajectory from an initializer list of samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   */
  SplineTrajectory(std::initializer_list<SplineSample> samples)
      : Trajectory<SplineSample, SplineTrajectory>(
            std::vector<SplineSample>(samples)) {}

  /**
   * Interpolates between two samples using constant-acceleration kinematic
   * equations.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  SplineSample Interpolate(const SplineSample& start, const SplineSample& end,
                           double t) const override;

  /**
   * Transforms all poses in the trajectory by the given transform.
   *
   * @param transform The transform to apply to the poses.
   * @return The transformed trajectory.
   */
  SplineTrajectory TransformBy(const Transform2d& transform) const override;

  /**
   * Transforms all poses so they are relative to the given pose.
   *
   * @param pose The pose to make the trajectory relative to.
   * @return The transformed trajectory.
   */
  SplineTrajectory RelativeTo(const Pose2d& pose) const override;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  SplineTrajectory Concatenate(
      const Trajectory<SplineSample, SplineTrajectory>& other) const override;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  SplineTrajectory operator+(const SplineTrajectory& other) const {
    return Concatenate(other);
  }
};

}  // namespace wpi::math
