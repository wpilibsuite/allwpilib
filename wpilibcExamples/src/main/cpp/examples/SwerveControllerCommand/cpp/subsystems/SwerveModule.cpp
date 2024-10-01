// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/SwerveModule.h"

#include <numbers>

#include <frc/geometry/Rotation2d.h>

#include "Constants.h"

SwerveModule::SwerveModule(int driveMotorChannel, int turningMotorChannel,
                           const int driveEncoderPorts[2],
                           const int turningEncoderPorts[2],
                           bool driveEncoderReversed,
                           bool turningEncoderReversed)
    : m_driveMotor(driveMotorChannel),
      m_turningMotor(turningMotorChannel),
      m_driveEncoder(driveEncoderPorts[0], driveEncoderPorts[1]),
      m_turningEncoder(turningEncoderPorts[0], turningEncoderPorts[1]) {
  // Set the distance per pulse for the drive encoder. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  m_driveEncoder.SetDistancePerPulse(
      ModuleConstants::kDriveEncoderDistancePerPulse);

  // Set whether drive encoder should be reversed or not
  m_driveEncoder.SetReverseDirection(driveEncoderReversed);

  // Set the distance (in this case, angle) per pulse for the turning encoder.
  // This is the the angle through an entire rotation (2 * std::numbers::pi)
  // divided by the encoder resolution.
  m_turningEncoder.SetDistancePerPulse(
      ModuleConstants::kTurningEncoderDistancePerPulse);

  // Set whether turning encoder should be reversed or not
  m_turningEncoder.SetReverseDirection(turningEncoderReversed);

  // Limit the PID Controller's input range between -pi and pi and set the input
  // to be continuous.
  m_turningPIDController.EnableContinuousInput(
      units::radian_t{-std::numbers::pi}, units::radian_t{std::numbers::pi});
}

frc::SwerveModuleState SwerveModule::GetState() {
  return {units::meters_per_second_t{m_driveEncoder.GetRate()},
          units::radian_t{m_turningEncoder.GetDistance()}};
}

frc::SwerveModulePosition SwerveModule::GetPosition() {
  return {units::meter_t{m_driveEncoder.GetDistance()},
          units::radian_t{m_turningEncoder.GetDistance()}};
}

void SwerveModule::SetDesiredState(frc::SwerveModuleState& referenceState) {
  frc::Rotation2d encoderRotation{
      units::radian_t{m_turningEncoder.GetDistance()}};

  // Optimize the reference state to avoid spinning further than 90 degrees
  referenceState.Optimize(encoderRotation);

  // Scale speed by cosine of angle error. This scales down movement
  // perpendicular to the desired direction of travel that can occur when
  // modules change directions. This results in smoother driving.
  referenceState.CosineScale(encoderRotation);

  // Calculate the drive output from the drive PID controller.
  const auto driveOutput = m_drivePIDController.Calculate(
      m_driveEncoder.GetRate(), referenceState.speed.value());

  // Calculate the turning motor output from the turning PID controller.
  auto turnOutput = m_turningPIDController.Calculate(
      units::radian_t{m_turningEncoder.GetDistance()},
      referenceState.angle.Radians());

  // Set the motor outputs.
  m_driveMotor.Set(driveOutput);
  m_turningMotor.Set(turnOutput);
}

void SwerveModule::ResetEncoders() {
  m_driveEncoder.Reset();
  m_turningEncoder.Reset();
}
