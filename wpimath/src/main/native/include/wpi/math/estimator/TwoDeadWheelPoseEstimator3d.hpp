// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/estimator/PoseEstimator3d.hpp"
#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/kinematics/TwoDeadWheelOdometry3d.hpp"
#include "wpi/math/kinematics/TwoDeadWheelPositions.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/array.hpp"

namespace wpi::math {
/**
 * This class wraps Double Follower Wheel Odometry to fuse latency-compensated
 * vision measurements with double follower wheel encoder distance measurements.
 * It will correct for noisy measurements and encoder drift. It is intended to
 * be a drop-in replacement for TwoDeadWheelOdometry3d. It is also
 * intended to be an easy replacement for TwoDeadWheelPoseEstimator,
 * only requiring the addition of a standard deviation for Z and appropriate
 * conversions between 2D and 3D versions of geometry classes. (See
 * Pose3d(Pose2d), Rotation3d(Rotation2d), Translation3d(Translation2d), and
 * Pose3d.ToPose2d().)
 *
 * Update() should be called every robot loop.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 */
class WPILIB_DLLEXPORT TwoDeadWheelPoseEstimator3d
    : public PoseEstimator3d<TwoDeadWheelPositions> {
 public:
  /**
   * Constructs a TwoDeadWheelPoseEstimator3d with default standard
   * deviations for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, 0.1 meters for z, and 0.1 radians for
   * angle. The default standard deviations of the vision measurements are
   * 0.9 meters for x, 0.9 meters for y, 0.9 meters for z, and 0.9 radians for
   * angle.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to
   *     the center of the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   *     center of the robot in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting pose estimate.
   */
  TwoDeadWheelPoseEstimator3d(
      wpi::units::meter_t xWheelYPos, wpi::units::meter_t yWheelXPos,
      const Rotation3d& gyroAngle,
      const TwoDeadWheelPositions& wheelPositions,
      const Pose3d& initialPose);

  /**
   * Constructs a TwoDeadWheelPoseEstimator3d.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to
   *     the center of the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   *     center of the robot in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, z position in meters, and angle in
   *     radians). Increase these numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, z position in
   *     meters, and angle in radians). Increase these numbers to trust the
   *     vision pose measurement less.
   */
  TwoDeadWheelPoseEstimator3d(
      wpi::units::meter_t xWheelYPos, wpi::units::meter_t yWheelXPos,
      const Rotation3d& gyroAngle,
      const TwoDeadWheelPositions& wheelPositions,
      const Pose3d& initialPose,
      const wpi::util::array<double, 4>& stateStdDevs,
      const wpi::util::array<double, 4>& visionMeasurementStdDevs);

 private:
  TwoDeadWheelOdometry3d m_odometryImpl;
};

}  // namespace wpi::math
