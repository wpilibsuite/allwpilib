// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/hardware/imu/OnboardIMU.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveOdometry.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"

/**
 * Represents a mecanum drive style drivetrain.
 */
class Drivetrain {
 public:
  Drivetrain() {
    imu.ResetYaw();
    // We need to invert one side of the drivetrain so that positive voltages
    // result in both sides moving forward. Depending on how your robot's
    // gearbox is constructed, you might have to invert the left side instead.
    frontRightMotor.SetInverted(true);
    backRightMotor.SetInverted(true);
  }

  wpi::math::MecanumDriveWheelPositions GetCurrentWheelDistances() const;
  wpi::math::MecanumDriveWheelVelocities GetCurrentWheelVelocities() const;
  void SetVelocities(
      const wpi::math::MecanumDriveWheelVelocities& wheelVelocities);
  void Drive(wpi::units::meters_per_second_t xVelocity,
             wpi::units::meters_per_second_t yVelocity,
             wpi::units::radians_per_second_t rot, bool fieldRelative,
             wpi::units::second_t period);
  void UpdateOdometry();

  static constexpr wpi::units::meters_per_second_t kMaxVelocity =
      3.0_mps;  // 3 meters per second
  static constexpr wpi::units::radians_per_second_t kMaxAngularVelocity{
      std::numbers::pi};  // 1/2 rotation per second

 private:
  wpi::PWMSparkMax frontLeftMotor{1};
  wpi::PWMSparkMax frontRightMotor{2};
  wpi::PWMSparkMax backLeftMotor{3};
  wpi::PWMSparkMax backRightMotor{4};

  wpi::Encoder frontLeftEncoder{0, 1};
  wpi::Encoder frontRightEncoder{2, 3};
  wpi::Encoder backLeftEncoder{4, 5};
  wpi::Encoder backRightEncoder{6, 7};

  wpi::math::Translation2d frontLeftLocation{0.381_m, 0.381_m};
  wpi::math::Translation2d frontRightLocation{0.381_m, -0.381_m};
  wpi::math::Translation2d backLeftLocation{-0.381_m, 0.381_m};
  wpi::math::Translation2d backRightLocation{-0.381_m, -0.381_m};

  wpi::math::PIDController frontLeftPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController frontRightPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController backLeftPIDController{1.0, 0.0, 0.0};
  wpi::math::PIDController backRightPIDController{1.0, 0.0, 0.0};

  wpi::OnboardIMU imu{wpi::OnboardIMU::FLAT};

  wpi::math::MecanumDriveKinematics kinematics{
      frontLeftLocation, frontRightLocation, backLeftLocation,
      backRightLocation};

  wpi::math::MecanumDriveOdometry odometry{kinematics, imu.GetRotation2d(),
                                           GetCurrentWheelDistances()};

  // Gains are for example purposes only - must be determined for your own
  // robot!
  wpi::math::SimpleMotorFeedforward<wpi::units::meters> feedforward{
      1_V, 3_V / 1_mps};
};
