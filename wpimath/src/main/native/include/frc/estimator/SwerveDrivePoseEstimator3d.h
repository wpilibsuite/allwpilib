// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/estimator/PoseEstimator3d.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveDriveOdometry3d.h"
#include "units/time.h"

namespace frc {

/**
 * This class wraps Swerve Drive Odometry to fuse latency-compensated
 * vision measurements with swerve drive encoder distance measurements. It is
 * intended to be a drop-in for SwerveDriveOdometry3d. It is also intended to be
 * an easy replacement for PoseEstimator, only requiring the addition of a
 * standard deviation for Z and appropriate conversions between 2D and 3D
 * versions of geometry classes. (See Pose3d(Pose2d), Rotation3d(Rotation2d),
 * Translation3d(Translation2d), and Pose3d.ToPose2d().)
 *
 * Update() should be called every robot loop.
 *
 * AddVisionMeasurement() can be called as infrequently as you want; if you
 * never call it, then this class will behave as regular encoder
 * odometry.
 */
template <size_t NumModules>
class SwerveDrivePoseEstimator3d
    : public PoseEstimator3d<wpi::array<SwerveModuleState, NumModules>,
                             wpi::array<SwerveModulePosition, NumModules>> {
 public:
  /**
   * Constructs a SwerveDrivePoseEstimator3d with default standard deviations
   * for the model and vision measurements.
   *
   * The default standard deviations of the model states are
   * 0.1 meters for x, 0.1 meters for y, 0.1 meters for z, and 0.1 radians for
   * angle. The default standard deviations of the vision measurements are 0.9
   * meters for x, 0.9 meters for y, 0.9 meters for z, and 0.9 radians for
   * angle.
   *
   * @param kinematics A correctly-configured kinematics object for your
   *     drivetrain.
   * @param gyroAngle The current gyro angle.
   * @param modulePositions The current distance and rotation measurements of
   *     the swerve modules.
   * @param initialPose The starting pose estimate.
   */
  SwerveDrivePoseEstimator3d(
      SwerveDriveKinematics<NumModules>& kinematics,
      const Rotation3d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose3d& initialPose)
      : SwerveDrivePoseEstimator3d{kinematics,           gyroAngle,
                                   modulePositions,      initialPose,
                                   {0.1, 0.1, 0.1, 0.1}, {0.9, 0.9, 0.9, 0.9}} {
  }

  /**
   * Constructs a SwerveDrivePoseEstimator3d.
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
  SwerveDrivePoseEstimator3d(
      SwerveDriveKinematics<NumModules>& kinematics,
      const Rotation3d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose3d& initialPose, const wpi::array<double, 4>& stateStdDevs,
      const wpi::array<double, 4>& visionMeasurementStdDevs)
      : SwerveDrivePoseEstimator3d::PoseEstimator3d(
            kinematics, m_odometryImpl, stateStdDevs, visionMeasurementStdDevs),
        m_odometryImpl{kinematics, gyroAngle, modulePositions, initialPose} {
    this->ResetPose(initialPose);
  }

 private:
  SwerveDriveOdometry3d<NumModules> m_odometryImpl;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDrivePoseEstimator3d<4>;

}  // namespace frc
