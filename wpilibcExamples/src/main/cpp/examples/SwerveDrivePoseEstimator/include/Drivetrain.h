// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <frc/AnalogGyro.h>
#include <frc/estimator/SwerveDrivePoseEstimator.h>
#include <frc/geometry/Translation2d.h>
#include <frc/kinematics/SwerveDriveKinematics.h>
#include <frc/kinematics/SwerveDriveOdometry.h>

#include "SwerveModule.h"

/**
 * Represents a swerve drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain() { m_gyro.Reset(); }

  void Drive(units::meters_per_second_t xSpeed,
             units::meters_per_second_t ySpeed, units::radians_per_second_t rot,
             bool fieldRelative, units::second_t period);
  void UpdateOdometry();

  static constexpr auto kMaxSpeed = 3.0_mps;  // 3 meters per second
  static constexpr units::radians_per_second_t kMaxAngularSpeed{
      std::numbers::pi};  // 1/2 rotation per second

 private:
  frc::Translation2d m_frontLeftLocation{+0.381_m, +0.381_m};
  frc::Translation2d m_frontRightLocation{+0.381_m, -0.381_m};
  frc::Translation2d m_backLeftLocation{-0.381_m, +0.381_m};
  frc::Translation2d m_backRightLocation{-0.381_m, -0.381_m};

  SwerveModule m_frontLeft{1, 2, 0, 1, 2, 3};
  SwerveModule m_frontRight{3, 4, 4, 5, 6, 7};
  SwerveModule m_backLeft{5, 6, 8, 9, 10, 11};
  SwerveModule m_backRight{7, 8, 12, 13, 14, 15};

  frc::AnalogGyro m_gyro{0};

  frc::SwerveDriveKinematics<4> m_kinematics{
      m_frontLeftLocation, m_frontRightLocation, m_backLeftLocation,
      m_backRightLocation};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  frc::SwerveDrivePoseEstimator<4> m_poseEstimator{
      m_kinematics,
      frc::Rotation2d{},
      {m_frontLeft.GetPosition(), m_frontRight.GetPosition(),
       m_backLeft.GetPosition(), m_backRight.GetPosition()},
      frc::Pose2d{},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1}};
};
