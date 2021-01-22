// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.h"

frc::MecanumDriveWheelSpeeds Drivetrain::GetCurrentState() const {
  return {units::meters_per_second_t(m_frontLeftEncoder.GetRate()),
          units::meters_per_second_t(m_frontRightEncoder.GetRate()),
          units::meters_per_second_t(m_backLeftEncoder.GetRate()),
          units::meters_per_second_t(m_backRightEncoder.GetRate())};
}

void Drivetrain::SetSpeeds(const frc::MecanumDriveWheelSpeeds& wheelSpeeds) {
  const auto frontLeftFeedforward =
      m_feedforward.Calculate(wheelSpeeds.frontLeft);
  const auto frontRightFeedforward =
      m_feedforward.Calculate(wheelSpeeds.frontRight);
  const auto backLeftFeedforward =
      m_feedforward.Calculate(wheelSpeeds.rearLeft);
  const auto backRightFeedforward =
      m_feedforward.Calculate(wheelSpeeds.rearRight);

  const double frontLeftOutput = m_frontLeftPIDController.Calculate(
      m_frontLeftEncoder.GetRate(), wheelSpeeds.frontLeft.to<double>());
  const double frontRightOutput = m_frontRightPIDController.Calculate(
      m_frontRightEncoder.GetRate(), wheelSpeeds.frontRight.to<double>());
  const double backLeftOutput = m_backLeftPIDController.Calculate(
      m_backLeftEncoder.GetRate(), wheelSpeeds.rearLeft.to<double>());
  const double backRightOutput = m_backRightPIDController.Calculate(
      m_backRightEncoder.GetRate(), wheelSpeeds.rearRight.to<double>());

  m_frontLeftMotor.SetVoltage(units::volt_t{frontLeftOutput} +
                              frontLeftFeedforward);
  m_frontRightMotor.SetVoltage(units::volt_t{frontRightOutput} +
                               frontRightFeedforward);
  m_backLeftMotor.SetVoltage(units::volt_t{backLeftOutput} +
                             backLeftFeedforward);
  m_backRightMotor.SetVoltage(units::volt_t{backRightOutput} +
                              backRightFeedforward);
}

void Drivetrain::Drive(units::meters_per_second_t xSpeed,
                       units::meters_per_second_t ySpeed,
                       units::radians_per_second_t rot, bool fieldRelative) {
  auto wheelSpeeds = m_kinematics.ToWheelSpeeds(
      fieldRelative ? frc::ChassisSpeeds::FromFieldRelativeSpeeds(
                          xSpeed, ySpeed, rot, m_gyro.GetRotation2d())
                    : frc::ChassisSpeeds{xSpeed, ySpeed, rot});
  wheelSpeeds.Normalize(kMaxSpeed);
  SetSpeeds(wheelSpeeds);
}

void Drivetrain::UpdateOdometry() {
  m_odometry.Update(m_gyro.GetRotation2d(), GetCurrentState());
}
