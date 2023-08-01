// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <cstddef>
#include <ctime>

#include <wpi/SymbolExports.h>
#include <wpi/timestamp.h>

#include "Odometry.h"
#include "SwerveDriveKinematics.h"
#include "SwerveDriveWheelPositions.h"
#include "SwerveModulePosition.h"
#include "SwerveModuleState.h"
#include "frc/geometry/Pose2d.h"
#include "units/time.h"

namespace frc {

/**
 * Class for swerve drive odometry. Odometry allows you to track the robot's
 * position on the field over a course of a match using readings from your
 * swerve drive encoders and swerve azimuth encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
template <size_t NumModules>
class SwerveDriveOdometry
    : public Odometry<SwerveDriveWheelSpeeds<NumModules>,
                      SwerveDriveWheelPositions<NumModules>> {
 public:
  /**
   * Constructs a SwerveDriveOdometry object.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   * @param initialPose The starting position of the robot on the field.
   */
  SwerveDriveOdometry(
      SwerveDriveKinematics<NumModules> kinematics, const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& initialPose = Pose2d{});

  /**
   * Resets the robot's position on the field.
   *
   * The gyroscope angle does not need to be reset here on the user's robot
   * code. The library automatically takes care of offsetting the gyro angle.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   * @param pose The position on the field that your robot is at.
   */
  void ResetPosition(
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& pose) {
    Odometry<
        SwerveDriveWheelSpeeds<NumModules>,
        SwerveDriveWheelPositions<NumModules>>::ResetPosition(gyroAngle,
                                                              {modulePositions},
                                                              pose);
  }

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This also takes in an angle parameter
   * which is used instead of the angular rate that is calculated from forward
   * kinematics.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The current position of all swerve modules. Please
   * provide the positions in the same order in which you instantiated your
   * SwerveDriveKinematics.
   *
   * @return The new pose of the robot.
   */
  const Pose2d& Update(
      const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions) {
    return Odometry<
        SwerveDriveWheelSpeeds<NumModules>,
        SwerveDriveWheelPositions<NumModules>>::Update(gyroAngle,
                                                       {modulePositions});
  }

 private:
  SwerveDriveKinematics<NumModules> m_kinematicsImpl;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDriveOdometry<4>;

}  // namespace frc

#include "SwerveDriveOdometry.inc"
