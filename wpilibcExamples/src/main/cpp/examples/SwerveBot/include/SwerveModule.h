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
#include <frc/kinematics/SwerveModuleState.h>
#include <wpi/math>

class SwerveModule {
 public:
  SwerveModule(int driveMotorChannel, int turningMotorChannel);
  frc::SwerveModuleState GetState() const;
  void SetDesiredState(const frc::SwerveModuleState& state);

 private:
  static constexpr double kWheelRadius = 0.0508;
  static constexpr int kEncoderResolution = 4096;

  // We have to use meters here instead of radians because of the fact that
  // ProfiledPIDController's constraints only take in meters per second and
  // meters per second squared.

  static constexpr units::meters_per_second_t kModuleMaxAngularVelocity =
      units::meters_per_second_t(wpi::math::pi);  // radians per second
  static constexpr units::meters_per_second_squared_t
      kModuleMaxAngularAcceleration = units::meters_per_second_squared_t(
          wpi::math::pi * 2.0);  // radians per second squared

  frc::Spark m_driveMotor;
  frc::Spark m_turningMotor;

  frc::Encoder m_driveEncoder{0, 1};
  frc::Encoder m_turningEncoder{0, 1};

  frc2::PIDController m_drivePIDController{1.0, 0, 0};
  frc::ProfiledPIDController m_turningPIDController{
      1.0,
      0.0,
      0.0,
      {kModuleMaxAngularVelocity, kModuleMaxAngularAcceleration}};
};
