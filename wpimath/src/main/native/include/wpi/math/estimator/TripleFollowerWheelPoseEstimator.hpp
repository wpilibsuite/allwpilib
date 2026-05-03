// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/estimator/PoseEstimator.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelOdometry.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelPositions.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/array.hpp"

namespace wpi::math {
/**
 * This class wraps Triple Follower Wheel Odometry to fuse latency-compensated
 * vision measurements with triple follower wheel encoder measurements. It is
 * intended to be a drop-in replacement for TripleFollowerWheelOdometry. In
 * fact, if you never call AddVisionMeasurement(), and only call Update(), this
 * will behave exactly the same as TripleFollowerWheelOdometry.
 *
 * Update() should be called every robot loop.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave like regular encoder odometry.
 */
class WPILIB_DLLEXPORT TripleFollowerWheelPoseEstimator
    : public PoseEstimator<TripleFollowerWheelPositions> {
 public:
  /**
   * Constructs a TripleFollowerWheelPoseEstimator with default standard
   * deviations for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   * The default standard deviations of the vision measurements are
   * 0.9 meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param x1WheelYPos The y-position of the first forward-facing wheel
   *     relative to the center of the robot in meters.
   * @param x2WheelYPos The y-position of the second forward-facing wheel
   *     relative to the center of the robot in meters.
   * @param yWheelXPos The x-position of the left-facing wheel relative to the
   *     center of the robot in meters.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The current wheel position readings.
   * @param initialPose The starting pose estimate.
   */
  TripleFollowerWheelPoseEstimator(
      wpi::units::meter_t x1WheelYPos, wpi::units::meter_t x2WheelYPos,
      wpi::units::meter_t yWheelXPos, const Rotation2d& gyroAngle,
      const TripleFollowerWheelPositions& wheelPositions,
      const Pose2d& initialPose);

  /**
   * Constructs a TripleFollowerWheelPoseEstimator.
   *
   * @param x1WheelYPos The y-position of the forward-facing wheel relative to
   *     the center of the robot in meters.
   * @param x2WheelYPos The y-position of the second forward-facing wheel
   *     relative to the center of the robot in meters.
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
  TripleFollowerWheelPoseEstimator(
      wpi::units::meter_t x1WheelYPos, wpi::units::meter_t x2WheelYPos,
      wpi::units::meter_t yWheelXPos, const Rotation2d& gyroAngle,
      const TripleFollowerWheelPositions& wheelPositions,
      const Pose2d& initialPose,
      const wpi::util::array<double, 3>& stateStdDevs,
      const wpi::util::array<double, 3>& visionMeasurementStdDevs);

 private:
  TripleFollowerWheelOdometry m_odometryImpl;
};

}  // namespace wpi::math
