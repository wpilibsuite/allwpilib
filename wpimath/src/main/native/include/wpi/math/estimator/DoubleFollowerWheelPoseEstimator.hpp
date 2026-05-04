// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/estimator/PoseEstimator.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/DoubleFollowerWheelOdometry.hpp"
#include "wpi/math/kinematics/DoubleFollowerWheelPositions.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/array.hpp"

namespace wpi::math {
/**
 * This class wraps Double Follower Wheel Odometry to fuse
 * latency-compensated vision measurements with double follower wheel encoder
 * measurements. It will correct for noisy vision measurements and encoder
 * drift. It is intended to be an easy drop-in for
 * DoubleFollowerWheelOdometry. In fact, if you never call
 * AddVisionMeasurement(), and only call Update(), this will behave exactly the
 * same as DoubleFollowerWheelOdometry.
 *
 * Update() should be called every robot loop.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave like regular encoder odometry.
 */
class WPILIB_DLLEXPORT DoubleFollowerWheelPoseEstimator
    : public PoseEstimator<DoubleFollowerWheelPositions> {
 public:
  /**
   * Constructs a DoubleFollowerWheelPoseEstimator with default standard
   * deviations for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   * The default standard deviations of the vision measurements are
   * 0.9 meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to
   *     the center of the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   *     center of the robot in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting pose estimate.
   */
  DoubleFollowerWheelPoseEstimator(
      wpi::units::meter_t xWheelYPos, wpi::units::meter_t yWheelXPos,
      const Rotation2d& gyroAngle,
      const DoubleFollowerWheelPositions& wheelPositions,
      const Pose2d& initialPose);

  /**
   * Constructs a DoubleFollowerWheelPoseEstimator.
   *
   * @param xWheelYPos The y-position of the forward-facing wheel relative to
   *     the center of the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   *     center of the robot in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, and heading in radians). Increase these
   *     numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  DoubleFollowerWheelPoseEstimator(
      wpi::units::meter_t xWheelYPos, wpi::units::meter_t yWheelXPos,
      const Rotation2d& gyroAngle,
      const DoubleFollowerWheelPositions& wheelPositions,
      const Pose2d& initialPose,
      const wpi::util::array<double, 3>& stateStdDevs,
      const wpi::util::array<double, 3>& visionMeasurementStdDevs);

 private:
  DoubleFollowerWheelOdometry m_odometryImpl;
};

}  // namespace wpi::math
