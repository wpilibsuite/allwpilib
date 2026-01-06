// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/estimator/PoseEstimator.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveOdometry.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/array.hpp"

namespace wpi::math {
/**
 * This class wraps Mecanum Drive Odometry to fuse latency-compensated
 * vision measurements with mecanum drive encoder velocity measurements. It will
 * correct for noisy measurements and encoder drift. It is intended to be an
 * easy drop-in for MecanumDriveOdometry.
 *
 * Update() should be called every robot loop. If your loops are faster or
 * slower than the default of 20 ms, then you should change the nominal delta
 * time by specifying it in the constructor.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave mostly like regular encoder
 * odometry.
 */
class WPILIB_DLLEXPORT MecanumDrivePoseEstimator
    : public PoseEstimator<MecanumDriveWheelPositions, MecanumDriveWheelSpeeds,
                           MecanumDriveWheelAccelerations> {
 public:
  /**
   * Constructs a MecanumDrivePoseEstimator with default standard deviations
   * for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   * The default standard deviations of the vision measurements are
   * 0.45 meters for x, 0.45 meters for y, and 0.45 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distance measured by each wheel.
   * @param initialPose The starting pose estimate.
   */
  MecanumDrivePoseEstimator(MecanumDriveKinematics& kinematics,
                            const Rotation2d& gyroAngle,
                            const MecanumDriveWheelPositions& wheelPositions,
                            const Pose2d& initialPose);

  /**
   * Constructs a MecanumDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param wheelPositions The distance measured by each wheel.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, and heading in radians). Increase these
   *     numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  MecanumDrivePoseEstimator(
      MecanumDriveKinematics& kinematics, const Rotation2d& gyroAngle,
      const MecanumDriveWheelPositions& wheelPositions,
      const Pose2d& initialPose,
      const wpi::util::array<double, 3>& stateStdDevs,
      const wpi::util::array<double, 3>& visionMeasurementStdDevs);

 private:
  MecanumDriveOdometry m_odometryImpl;
};

}  // namespace wpi::math
