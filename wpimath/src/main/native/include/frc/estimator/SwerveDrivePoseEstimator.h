// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/estimator/PoseEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveDriveOdometry.h"
#include "units/time.h"

namespace frc {

/**
 * This class wraps Swerve Drive Odometry to fuse latency-compensated
 * vision measurements with swerve drive encoder distance measurements. It is
 * intended to be a drop-in for SwerveDriveOdometry.
 *
 * Update() should be called every robot loop.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave as regular encoder
 * odometry.
 */
template <size_t NumModules>
class SwerveDrivePoseEstimator
    : public PoseEstimator<wpi::array<SwerveModuleState, NumModules>,
                           wpi::array<SwerveModulePosition, NumModules>> {
 public:
  /**
   * Constructs a SwerveDrivePoseEstimator with default standard deviations
   * for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, and 0.1 radians for heading.
   * The default standard deviations of the vision measurements are
   * 0.9 meters for x, 0.9 meters for y, and 0.9 radians for heading.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of
   *     the swerve modules.
   * @param initialPose The starting pose estimate.
   */
  SwerveDrivePoseEstimator(
      SwerveDriveKinematics<NumModules>& kinematics,
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& initialPose)
      : SwerveDrivePoseEstimator{kinematics,      gyroAngle,
                                 modulePositions, initialPose,
                                 {0.1, 0.1, 0.1}, {0.9, 0.9, 0.9}} {}

  /**
   * Constructs a SwerveDrivePoseEstimator.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of
   *     the swerve modules.
   * @param initialPose The starting pose estimate.
   * @param stateStdDevs Standard deviations of the pose estimate (x position in
   *     meters, y position in meters, and heading in radians). Increase these
   *     numbers to trust your state estimate less.
   * @param visionMeasurementStdDevs Standard deviations of the vision pose
   *     measurement (x position in meters, y position in meters, and heading in
   *     radians). Increase these numbers to trust the vision pose measurement
   *     less.
   */
  SwerveDrivePoseEstimator(
      SwerveDriveKinematics<NumModules>& kinematics,
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& initialPose, const wpi::array<double, 3>& stateStdDevs,
      const wpi::array<double, 3>& visionMeasurementStdDevs)
      : SwerveDrivePoseEstimator::PoseEstimator(
            kinematics, m_odometryImpl, stateStdDevs, visionMeasurementStdDevs),
        m_odometryImpl{kinematics, gyroAngle, modulePositions, initialPose} {
    this->ResetPose(initialPose);
  }

 private:
  SwerveDriveOdometry<NumModules> m_odometryImpl;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDrivePoseEstimator<4>;

}  // namespace frc
