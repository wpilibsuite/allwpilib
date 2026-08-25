// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/drivers/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/math/controller/PIDController.hpp"
#include "wpi/math/controller/ProfiledPIDController.hpp"
#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/kinematics/SwerveModulePosition.hpp"
#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

class SwerveModule {
 public:
  SwerveModule(int driveMotorChannel, int turningMotorChannel,
               int driveEncoderChannelA, int driveEncoderChannelB,
               int turningEncoderChannelA, int turningEncoderChannelB);
  wpi::math::SwerveModulePosition GetPosition() const;
  wpi::math::SwerveModuleVelocity GetVelocity() const;
  void SetDesiredVelocity(wpi::math::SwerveModuleVelocity& desiredVelocity);

 private:
  static constexpr auto WHEEL_RADIUS = 0.0508_m;
  static constexpr int ENCODER_RESOLUTION = 4096;

  static constexpr auto MODULE_MAX_ANGULAR_VELOCITY =
      std::numbers::pi * 1_rad_per_s;  // radians per second
  static constexpr auto MODULE_MAX_ANGULAR_ACCELERATION =
      std::numbers::pi * 2_rad_per_s / 1_s;  // radians per second^2

  wpi::PWMSparkMax driveMotor;
  wpi::PWMSparkMax turningMotor;

  wpi::Encoder driveEncoder;
  wpi::Encoder turningEncoder;

  wpi::math::PIDController drivePIDController{1.0, 0, 0};
  wpi::math::ProfiledPIDController<wpi::units::radians_> turningPIDController{
      1.0,
      0.0,
      0.0,
      {MODULE_MAX_ANGULAR_VELOCITY, MODULE_MAX_ANGULAR_ACCELERATION}};

  wpi::math::SimpleMotorFeedforward<wpi::units::meters_> driveFeedforward{
      1_V, 3_V / 1_mps};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians_> turnFeedforward{
      1_V, 0.5_V / 1_rad_per_s};
};
