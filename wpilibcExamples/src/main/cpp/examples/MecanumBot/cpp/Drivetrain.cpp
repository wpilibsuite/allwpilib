// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include "wpi/math/kinematics/ChassisVelocities.hpp"

wpi::math::MecanumDriveWheelPositions Drivetrain::GetCurrentWheelDistances()
    const {
  return {wpi::units::meter_t{m_frontLeftEncoder.GetDistance()},
          wpi::units::meter_t{m_frontRightEncoder.GetDistance()},
          wpi::units::meter_t{m_backLeftEncoder.GetDistance()},
          wpi::units::meter_t{m_backRightEncoder.GetDistance()}};
}

wpi::math::MecanumDriveWheelVelocities Drivetrain::GetCurrentWheelVelocities()
    const {
  return {wpi::units::meters_per_second_t{m_frontLeftEncoder.GetRate()},
          wpi::units::meters_per_second_t{m_frontRightEncoder.GetRate()},
          wpi::units::meters_per_second_t{m_backLeftEncoder.GetRate()},
          wpi::units::meters_per_second_t{m_backRightEncoder.GetRate()}};
}

void Drivetrain::SetVelocities(
    const wpi::math::MecanumDriveWheelVelocities& wheelVelocities) {
  const auto frontLeftFeedforward =
      m_feedforward.Calculate(wheelVelocities.frontLeft);
  const auto frontRightFeedforward =
      m_feedforward.Calculate(wheelVelocities.frontRight);
  const auto backLeftFeedforward =
      m_feedforward.Calculate(wheelVelocities.rearLeft);
  const auto backRightFeedforward =
      m_feedforward.Calculate(wheelVelocities.rearRight);

  const double frontLeftOutput = m_frontLeftPIDController.Calculate(
      m_frontLeftEncoder.GetRate(), wheelVelocities.frontLeft.value());
  const double frontRightOutput = m_frontRightPIDController.Calculate(
      m_frontRightEncoder.GetRate(), wheelVelocities.frontRight.value());
  const double backLeftOutput = m_backLeftPIDController.Calculate(
      m_backLeftEncoder.GetRate(), wheelVelocities.rearLeft.value());
  const double backRightOutput = m_backRightPIDController.Calculate(
      m_backRightEncoder.GetRate(), wheelVelocities.rearRight.value());

  m_frontLeftMotor.SetVoltage(wpi::units::volt_t{frontLeftOutput} +
                              frontLeftFeedforward);
  m_frontRightMotor.SetVoltage(wpi::units::volt_t{frontRightOutput} +
                               frontRightFeedforward);
  m_backLeftMotor.SetVoltage(wpi::units::volt_t{backLeftOutput} +
                             backLeftFeedforward);
  m_backRightMotor.SetVoltage(wpi::units::volt_t{backRightOutput} +
                              backRightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xVelocity,
                       wpi::units::meters_per_second_t yVelocity,
                       wpi::units::radians_per_second_t rot, bool fieldRelative,
                       wpi::units::second_t period) {
  wpi::math::ChassisVelocities chassisVelocities{xVelocity, yVelocity, rot};
  if (fieldRelative) {
    chassisVelocities =
        chassisVelocities.ToRobotRelative(m_imu.GetRotation2d());
  }
  SetVelocities(
      m_kinematics.ToWheelVelocities(chassisVelocities.Discretize(period))
          .Desaturate(kMaxVelocity));
}

void Drivetrain::UpdateOdometry() {
  m_odometry.Update(m_imu.GetRotation2d(), GetCurrentWheelDistances());
}
