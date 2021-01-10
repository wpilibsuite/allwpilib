// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/SwerveModule.h"

#include <frc/geometry/Rotation2d.h>
#include <wpi/math>

#include "Constants.h"

SwerveModule::SwerveModule(int driveMotorChannel, int turningMotorChannel,
                           const int driveEncoderPorts[],
                           const int turningEncoderPorts[],
                           bool driveEncoderReversed,
                           bool turningEncoderReversed)
    : m_driveMotor(driveMotorChannel),
      m_turningMotor(turningMotorChannel),
      m_driveEncoder(driveEncoderPorts[0], driveEncoderPorts[1]),
      m_turningEncoder(turningEncoderPorts[0], turningEncoderPorts[1]),
      m_reverseDriveEncoder(driveEncoderReversed),
      m_reverseTurningEncoder(turningEncoderReversed) {
  // Set the distance per pulse for the drive encoder. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  m_driveEncoder.SetDistancePerPulse(
      ModuleConstants::kDriveEncoderDistancePerPulse);

  // Set the distance (in this case, angle) per pulse for the turning encoder.
  // This is the the angle through an entire rotation (2 * wpi::math::pi)
  // divided by the encoder resolution.
  m_turningEncoder.SetDistancePerPulse(
      ModuleConstants::kTurningEncoderDistancePerPulse);

  // Limit the PID Controller's input range between -pi and pi and set the input
  // to be continuous.
  m_turningPIDController.EnableContinuousInput(units::radian_t(-wpi::math::pi),
                                               units::radian_t(wpi::math::pi));
}

frc::SwerveModuleState SwerveModule::GetState() {
  return {units::meters_per_second_t{m_driveEncoder.GetRate()},
          frc::Rotation2d(units::radian_t(m_turningEncoder.Get()))};
}

void SwerveModule::SetDesiredState(
    const frc::SwerveModuleState& referenceState) {
  // Optimize the reference state to avoid spinning further than 90 degrees
  const auto state = frc::SwerveModuleState::Optimize(
      referenceState, units::radian_t(m_turningEncoder.Get()));

  // Calculate the drive output from the drive PID controller.
  const auto driveOutput = m_drivePIDController.Calculate(
      m_driveEncoder.GetRate(), state.speed.to<double>());

  // Calculate the turning motor output from the turning PID controller.
  auto turnOutput = m_turningPIDController.Calculate(
      units::radian_t(m_turningEncoder.Get()), state.angle.Radians());

  // Set the motor outputs.
  m_driveMotor.Set(driveOutput);
  m_turningMotor.Set(turnOutput);
}

void SwerveModule::ResetEncoders() {
  m_driveEncoder.Reset();
  m_turningEncoder.Reset();
}
