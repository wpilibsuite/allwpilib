/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/Encoder.h>
#include <frc/Spark.h>
#include <frc/controller/PIDController.h>
#include <frc/controller/ProfiledPIDController.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/kinematics/SwerveModuleState.h>
#include <frc/trajectory/TrapezoidProfile.h>
#include <wpi/math>

#include "Constants.h"

class SwerveModule {
 public:
  SwerveModule(int driveMotorChannel, int turningMotorChannel,
               std::array<int, 2> driveEncoderPorts,
               std::array<int, 2> turningEncoderPorts,
               bool driveEncoderReversed, bool turningEncoderReversed);

  frc::SwerveModuleState GetState();

  void SetDesiredState(frc::SwerveModuleState& state);

  void ResetEncoders();

 private:
  // We have to use meters here instead of radians due to the fact that
  // ProfiledPIDController's constraints only take in meters per second and
  // meters per second squared.

  static constexpr units::meters_per_second_t kModuleMaxAngularVelocity =
      units::meters_per_second_t(wpi::math::pi);  // radians per second
  static constexpr units::meters_per_second_squared_t
      kModuleMaxAngularAcceleration = units::meters_per_second_squared_t(
          wpi::math::pi * 2.0);  // radians per second squared

  frc::Spark m_driveMotor;
  frc::Spark m_turningMotor;

  frc::Encoder m_driveEncoder;
  frc::Encoder m_turningEncoder;

  bool m_reverseDriveEncoder;
  bool m_reverseTurningEncoder;

  frc2::PIDController m_drivePIDController{
      ModuleConstants::kPModuleDriveController, 0, 0};
  frc::ProfiledPIDController m_turningPIDController{
      ModuleConstants::kPModuleTurningController,
      0.0,
      0.0,
      {kModuleMaxAngularVelocity, kModuleMaxAngularAcceleration}};
};