// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <frc/Encoder.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/kinematics/SwerveModulePosition.h>
#include <frc/kinematics/SwerveModuleState.h>
#include <frc/motorcontrol/Spark.h>
#include <frc/trajectory/TrapezoidProfile.h>

#include "Constants.h"

class SwerveModule {
 public:
  SwerveModule(int driveMotorChannel, int turningMotorChannel,
               const int driveEncoderPorts[2], const int turningEncoderPorts[2],
               bool driveEncoderReversed, bool turningEncoderReversed);

  frc::SwerveModuleState GetState();

  frc::SwerveModulePosition GetPosition();

  void SetDesiredState(frc::SwerveModuleState& state);

  void ResetEncoders();

 private:
  // We have to use meters here instead of radians due to the fact that
  // ProfiledPIDController's constraints only take in meters per second and
  // meters per second squared.

  static constexpr auto MODULE_MAX_ANGULAR_VELOCITY =
      units::radians_per_second_t{std::numbers::pi};
  static constexpr auto MODULE_MAX_ANGULAR_ACCELERATION =
      units::radians_per_second_squared_t{std::numbers::pi * 2.0};

  frc::Spark m_driveMotor;
  frc::Spark m_turningMotor;

  frc::Encoder m_driveEncoder;
  frc::Encoder m_turningEncoder;

  frc::PIDController m_drivePIDController{
      ModuleConstants::MODULE_DRIVE_CONTROLLER_P, 0, 0};
  frc::ProfiledPIDController<units::radians> m_turningPIDController{
      ModuleConstants::MODULE_TURNING_CONTROLLER_P,
      0.0,
      0.0,
      {MODULE_MAX_ANGULAR_VELOCITY, MODULE_MAX_ANGULAR_ACCELERATION}};
};
