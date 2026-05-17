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
    : driveMotor(driveMotorChannel),
      turningMotor(turningMotorChannel),
      driveEncoder(driveEncoderChannelA, driveEncoderChannelB),
      turningEncoder(turningEncoderChannelA, turningEncoderChannelB) {
  // Set the distance per pulse for the drive encoder. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  driveEncoder.SetDistancePerPulse(2 * std::numbers::pi * kWheelRadius /
                                   kEncoderResolution);

  // Set the distance (in this case, angle) per pulse for the turning encoder.
  // This is the the angle through an entire rotation (2 * std::numbers::pi)
  // divided by the encoder resolution.
  turningEncoder.SetDistancePerPulse(2 * std::numbers::pi / kEncoderResolution);

  // Limit the PID Controller's input range between -pi and pi and set the input
  // to be continuous.
  turningPIDController.EnableContinuousInput(
      -wpi::units::radian_t{std::numbers::pi},
      wpi::units::radian_t{std::numbers::pi});
}

wpi::math::SwerveModulePosition SwerveModule::GetPosition() const {
  return {wpi::units::meter_t{driveEncoder.GetDistance()},
          wpi::units::radian_t{turningEncoder.GetDistance()}};
}

wpi::math::SwerveModuleVelocity SwerveModule::GetVelocity() const {
  return {wpi::units::meters_per_second_t{driveEncoder.GetRate()},
          wpi::units::radian_t{turningEncoder.GetDistance()}};
}

void SwerveModule::SetDesiredVelocity(
    wpi::math::SwerveModuleVelocity& desiredVelocity) {
  wpi::math::Rotation2d encoderRotation{
      wpi::units::radian_t{turningEncoder.GetDistance()}};

  // Optimize the desired velocity to avoid spinning further than 90 degrees,
  // then scale velocity by cosine of angle error. This scales down movement
  // perpendicular to the desired direction of travel that can occur when
  // modules change directions. This results in smoother driving.
  auto velocity =
      desiredVelocity.Optimize(encoderRotation).CosineScale(encoderRotation);

  // Calculate the drive output from the drive PID controller and feedforward.
  const auto driveOutput =
      wpi::units::volt_t{drivePIDController.Calculate(
          driveEncoder.GetRate(), velocity.velocity.value())} +
      driveFeedforward.Calculate(velocity.velocity);

  // Calculate the turning motor output from the turning PID controller and
  // feedforward.
  const auto turnOutput =
      wpi::units::volt_t{turningPIDController.Calculate(
          wpi::units::radian_t{turningEncoder.GetDistance()},
          velocity.angle.Radians())} +
      turnFeedforward.Calculate(turningPIDController.GetSetpoint().velocity);

  // Set the motor outputs.
  driveMotor.SetVoltage(driveOutput);
  turningMotor.SetVoltage(turnOutput);
}
