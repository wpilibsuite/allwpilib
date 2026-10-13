// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

void Drivetrain::SetVelocities(
    const wpi::math::DifferentialDriveWheelVelocities& velocities) {
  const auto leftFeedforward = feedforward.Calculate(velocities.left);
  const auto rightFeedforward = feedforward.Calculate(velocities.right);
  const double leftOutput = leftPIDController.Calculate(
      leftEncoder.GetRate(), velocities.left.value());
  const double rightOutput = rightPIDController.Calculate(
      rightEncoder.GetRate(), velocities.right.value());

  leftLeader.SetVoltage(wpi::units::volt_t{leftOutput} + leftFeedforward);
  rightLeader.SetVoltage(wpi::units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xVelocity,
                       wpi::units::radians_per_second_t rot) {
  SetVelocities(kinematics.ToWheelVelocities({xVelocity, 0_mps, rot}));
}

void Drivetrain::UpdateOdometry() {
  odometry.Update(imu.GetRotation2d(),
                  wpi::units::meter_t{leftEncoder.GetDistance()},
                  wpi::units::meter_t{rightEncoder.GetDistance()});
}
