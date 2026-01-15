// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SwerveModule.hpp"

#include <numbers>

#include "wpi/math/geometry/Rotation2d.hpp"

SwerveModule::SwerveModule(const int driveMotorChannel,
                           const int turningMotorChannel,
                           const int driveEncoderChannelA,
                           const int driveEncoderChannelB,
                           const int turningEncoderChannelA,
                           const int turningEncoderChannelB)
    : m_driveMotor(driveMotorChannel),
      m_turningMotor(turningMotorChannel),
      m_driveEncoder(driveEncoderChannelA, driveEncoderChannelB),
      m_turningEncoder(turningEncoderChannelA, turningEncoderChannelB) {
  // Set the distance per pulse for the drive encoder. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  m_driveEncoder.SetDistancePerPulse(2 * std::numbers::pi *
                                     kWheelRadius.value() / kEncoderResolution);

  // Set the distance (in this case, angle) per pulse for the turning encoder.
  // This is the the angle through an entire rotation (2 * std::numbers::pi)
  // divided by the encoder resolution.
  m_turningEncoder.SetDistancePerPulse(2 * std::numbers::pi /
                                       kEncoderResolution);

  // Limit the PID Controller's input range between -pi and pi and set the input
  // to be continuous.
  m_turningPIDController.EnableContinuousInput(
      -wpi::units::radian_t{std::numbers::pi},
      wpi::units::radian_t{std::numbers::pi});
}

wpi::math::SwerveModulePosition SwerveModule::GetPosition() const {
  return {wpi::units::meter_t{m_driveEncoder.GetDistance()},
          wpi::units::radian_t{m_turningEncoder.GetDistance()}};
}

wpi::math::SwerveModuleVelocity SwerveModule::GetVelocity() const {
  return {wpi::units::meters_per_second_t{m_driveEncoder.GetRate()},
          wpi::units::radian_t{m_turningEncoder.GetDistance()}};
}

void SwerveModule::SetDesiredVelocity(
    wpi::math::SwerveModuleVelocity& desiredVelocity) {
  wpi::math::Rotation2d encoderRotation{
      wpi::units::radian_t{m_turningEncoder.GetDistance()}};

  // Optimize the desired velocity to avoid spinning further than 90 degrees
  desiredVelocity.Optimize(encoderRotation);

  // Scale velocity by cosine of angle error. This scales down movement
  // perpendicular to the desired direction of travel that can occur when
  // modules change directions. This results in smoother driving.
  desiredVelocity.CosineScale(encoderRotation);

  // Calculate the drive output from the drive PID controller.
  const auto driveOutput = m_drivePIDController.Calculate(
      m_driveEncoder.GetRate(), desiredVelocity.velocity.value());

  const auto driveFeedforward =
      m_driveFeedforward.Calculate(desiredVelocity.velocity);

  // Calculate the turning motor output from the turning PID controller.
  const auto turnOutput = m_turningPIDController.Calculate(
      wpi::units::radian_t{m_turningEncoder.GetDistance()},
      desiredVelocity.angle.Radians());

  const auto turnFeedforward = m_turnFeedforward.Calculate(
      m_turningPIDController.GetSetpoint().velocity);

  // Set the motor outputs.
  m_driveMotor.SetVoltage(wpi::units::volt_t{driveOutput} + driveFeedforward);
  m_turningMotor.SetVoltage(wpi::units::volt_t{turnOutput} + turnFeedforward);
}
