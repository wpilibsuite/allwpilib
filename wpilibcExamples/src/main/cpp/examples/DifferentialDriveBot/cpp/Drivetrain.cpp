// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

void Drivetrain::SetVelocities(
    const wpi::math::DifferentialDriveWheelVelocities& velocities) {
  const auto leftFeedforward = m_feedforward.Calculate(velocities.left);
  const auto rightFeedforward = m_feedforward.Calculate(velocities.right);
  const double leftOutput = m_leftPIDController.Calculate(
      m_leftEncoder.GetRate(), velocities.left.value());
  const double rightOutput = m_rightPIDController.Calculate(
      m_rightEncoder.GetRate(), velocities.right.value());

  m_leftLeader.SetVoltage(wpi::units::volt_t{leftOutput} + leftFeedforward);
  m_rightLeader.SetVoltage(wpi::units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xVelocity,
                       wpi::units::radians_per_second_t rot) {
  SetVelocities(m_kinematics.ToWheelVelocities({xVelocity, 0_mps, rot}));
}

void Drivetrain::UpdateOdometry() {
  m_odometry.Update(m_imu.GetRotation2d(),
                    wpi::units::meter_t{m_leftEncoder.GetDistance()},
                    wpi::units::meter_t{m_rightEncoder.GetDistance()});
}
