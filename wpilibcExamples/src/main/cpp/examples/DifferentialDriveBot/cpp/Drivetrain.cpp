// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

void Drivetrain::SetSpeeds(const wpi::math::DifferentialDriveWheelSpeeds& speeds) {
  const auto leftFeedforward = m_feedforward.Calculate(speeds.left);
  const auto rightFeedforward = m_feedforward.Calculate(speeds.right);
  const double leftOutput = m_leftPIDController.Calculate(
      m_leftEncoder.GetRate(), speeds.left.value());
  const double rightOutput = m_rightPIDController.Calculate(
      m_rightEncoder.GetRate(), speeds.right.value());

  m_leftLeader.SetVoltage(wpi::units::volt_t{leftOutput} + leftFeedforward);
  m_rightLeader.SetVoltage(wpi::units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xSpeed,
                       wpi::units::radians_per_second_t rot) {
  SetSpeeds(m_kinematics.ToWheelSpeeds({xSpeed, 0_mps, rot}));
}

void Drivetrain::UpdateOdometry() {
  m_odometry.Update(m_imu.GetRotation2d(),
                    wpi::units::meter_t{m_leftEncoder.GetDistance()},
                    wpi::units::meter_t{m_rightEncoder.GetDistance()});
}
