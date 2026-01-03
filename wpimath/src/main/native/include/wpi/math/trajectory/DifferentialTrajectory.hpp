// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * A trajectory for differential drive robots with drivetrain-specific
 * interpolation.
 *
 * This trajectory uses numerical integration of the differential drive
 * differential equation for accurate interpolation.
 */
class WPILIB_DLLEXPORT DifferentialTrajectory
    : public Trajectory<DifferentialSample> {
 public:
  /**
   * Constructs a DifferentialTrajectory from a vector of samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   * @throws std::invalid_argument if the vector of samples is empty.
   */
  explicit DifferentialTrajectory(std::vector<DifferentialSample> samples)
      : Trajectory<DifferentialSample>(std::move(samples)) {}

  /**
   * Interpolates between two samples using numerical integration of the
   * differential drive differential equation.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  DifferentialSample Interpolate(const DifferentialSample& start,
                                 const DifferentialSample& end,
                                 double t) const final;

  /**
   * Transforms all poses in the trajectory by the given transform.
   *
   * @param transform The transform to apply to the poses.
   * @return The transformed trajectory.
   */
  DifferentialTrajectory TransformBy(const Transform2d& transform) const;

  /**
   * Transforms all poses so they are relative to the given pose.
   *
   * @param pose The pose to make the trajectory relative to.
   * @return The transformed trajectory.
   */
  DifferentialTrajectory RelativeTo(const Pose2d& pose) const;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  DifferentialTrajectory Concatenate(const DifferentialTrajectory& other) const;

  /**
   * Concatenates another trajectory to this trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  DifferentialTrajectory operator+(const DifferentialTrajectory& other) const {
    return Concatenate(other);
  }
};

}  // namespace wpi::math
