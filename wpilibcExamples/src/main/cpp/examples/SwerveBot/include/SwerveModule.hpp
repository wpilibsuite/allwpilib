// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include "wpi/hardware/motor/PWMSparkMax.hpp"
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
  static constexpr double kWheelRadius = 0.0508;
  static constexpr int kEncoderResolution = 4096;

  static constexpr auto kModuleMaxAngularVelocity =
      std::numbers::pi * 1_rad_per_s;  // radians per second
  static constexpr auto kModuleMaxAngularAcceleration =
      std::numbers::pi * 2_rad_per_s / 1_s;  // radians per second^2

  wpi::PWMSparkMax m_driveMotor;
  wpi::PWMSparkMax m_turningMotor;

  wpi::Encoder m_driveEncoder;
  wpi::Encoder m_turningEncoder;

  wpi::math::PIDController m_drivePIDController{1.0, 0, 0};
  wpi::math::ProfiledPIDController<wpi::units::radians> m_turningPIDController{
      1.0,
      0.0,
      0.0,
      {kModuleMaxAngularVelocity, kModuleMaxAngularAcceleration}};

  wpi::math::SimpleMotorFeedforward<wpi::units::meters> m_driveFeedforward{
      1_V, 3_V / 1_mps};
  wpi::math::SimpleMotorFeedforward<wpi::units::radians> m_turnFeedforward{
      1_V, 0.5_V / 1_rad_per_s};
};
