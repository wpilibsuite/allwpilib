// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include "wpi/math/kinematics/ChassisVelocities.hpp"

wpi::math::MecanumDriveWheelPositions Drivetrain::GetCurrentWheelDistances()
    const {
  return {wpi::units::meter_t{frontLeftEncoder.GetDistance()},
          wpi::units::meter_t{frontRightEncoder.GetDistance()},
          wpi::units::meter_t{backLeftEncoder.GetDistance()},
          wpi::units::meter_t{backRightEncoder.GetDistance()}};
}

wpi::math::MecanumDriveWheelVelocities Drivetrain::GetCurrentWheelVelocities()
    const {
  return {wpi::units::meters_per_second_t{frontLeftEncoder.GetRate()},
          wpi::units::meters_per_second_t{frontRightEncoder.GetRate()},
          wpi::units::meters_per_second_t{backLeftEncoder.GetRate()},
          wpi::units::meters_per_second_t{backRightEncoder.GetRate()}};
}

void Drivetrain::SetVelocities(
    const wpi::math::MecanumDriveWheelVelocities& wheelVelocities) {
  const auto frontLeftFeedforward =
      feedforward.Calculate(wheelVelocities.frontLeft);
  const auto frontRightFeedforward =
      feedforward.Calculate(wheelVelocities.frontRight);
  const auto backLeftFeedforward =
      feedforward.Calculate(wheelVelocities.rearLeft);
  const auto backRightFeedforward =
      feedforward.Calculate(wheelVelocities.rearRight);

  const double frontLeftOutput = frontLeftPIDController.Calculate(
      frontLeftEncoder.GetRate(), wheelVelocities.frontLeft.value());
  const double frontRightOutput = frontRightPIDController.Calculate(
      frontRightEncoder.GetRate(), wheelVelocities.frontRight.value());
  const double backLeftOutput = backLeftPIDController.Calculate(
      backLeftEncoder.GetRate(), wheelVelocities.rearLeft.value());
  const double backRightOutput = backRightPIDController.Calculate(
      backRightEncoder.GetRate(), wheelVelocities.rearRight.value());

  frontLeftMotor.SetVoltage(wpi::units::volt_t{frontLeftOutput} +
                            frontLeftFeedforward);
  frontRightMotor.SetVoltage(wpi::units::volt_t{frontRightOutput} +
                             frontRightFeedforward);
  backLeftMotor.SetVoltage(wpi::units::volt_t{backLeftOutput} +
                           backLeftFeedforward);
  backRightMotor.SetVoltage(wpi::units::volt_t{backRightOutput} +
                            backRightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xVelocity,
                       wpi::units::meters_per_second_t yVelocity,
                       wpi::units::radians_per_second_t rot, bool fieldRelative,
                       wpi::units::second_t period) {
  wpi::math::ChassisVelocities chassisVelocities{xVelocity, yVelocity, rot};
  if (fieldRelative) {
    chassisVelocities = chassisVelocities.ToRobotRelative(imu.GetRotation2d());
  }
  SetVelocities(
      kinematics.ToWheelVelocities(chassisVelocities.Discretize(period))
          .Desaturate(kMaxVelocity));
}

void Drivetrain::UpdateOdometry() {
  odometry.Update(imu.GetRotation2d(), GetCurrentWheelDistances());
}
