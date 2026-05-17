// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "SwerveModule.hpp"
#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/math/estimator/SwerveDrivePoseEstimator.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"

/**
 * Represents a swerve drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain() { imu.ResetYaw(); }

  void Drive(wpi::units::meters_per_second_t xVelocity,
             wpi::units::meters_per_second_t yVelocity,
             wpi::units::radians_per_second_t rot, bool fieldRelative,
             wpi::units::second_t period);
  void UpdateOdometry();

  static constexpr auto kMaxVelocity = 3.0_mps;  // 3 meters per second
  static constexpr wpi::units::radians_per_second_t kMaxAngularVelocity{
      std::numbers::pi};  // 1/2 rotation per second

 private:
  wpi::math::Translation2d frontLeftLocation{+0.381_m, +0.381_m};
  wpi::math::Translation2d frontRightLocation{+0.381_m, -0.381_m};
  wpi::math::Translation2d backLeftLocation{-0.381_m, +0.381_m};
  wpi::math::Translation2d backRightLocation{-0.381_m, -0.381_m};

  SwerveModule frontLeft{1, 2, 0, 1, 2, 3};
  SwerveModule frontRight{3, 4, 4, 5, 6, 7};
  SwerveModule backLeft{5, 6, 8, 9, 10, 11};
  SwerveModule backRight{7, 8, 12, 13, 14, 15};

  wpi::OnboardIMU imu{wpi::OnboardIMU::FLAT};

  wpi::math::SwerveDriveKinematics<4> kinematics{
      frontLeftLocation, frontRightLocation, backLeftLocation,
      backRightLocation};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::SwerveDrivePoseEstimator<4> poseEstimator{
      kinematics,
      wpi::math::Rotation2d{},
      {frontLeft.GetPosition(), frontRight.GetPosition(),
       backLeft.GetPosition(), backRight.GetPosition()},
      wpi::math::Pose2d{},
      {0.1, 0.1, 0.1},
      {0.1, 0.1, 0.1}};
};
