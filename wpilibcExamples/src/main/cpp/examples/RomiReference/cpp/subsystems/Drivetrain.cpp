// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drivetrain.hpp"

#include <numbers>

#include "Constants.hpp"

using namespace DriveConstants;

// The Romi has the left and right motors set to
// PWM channels 0 and 1 respectively
// The Romi has onboard encoders that are hardcoded
// to use DIO pins 4/5 and 6/7 for the left and right
Drivetrain::Drivetrain() {
  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  rightMotor.SetInverted(true);

  leftEncoder.SetDistancePerPulse(std::numbers::pi * WHEEL_DIAMETER.value() /
                                  COUNTS_PER_REVOLUTION);
  rightEncoder.SetDistancePerPulse(std::numbers::pi * WHEEL_DIAMETER.value() /
                                   COUNTS_PER_REVOLUTION);
  ResetEncoders();
}

void Drivetrain::Periodic() {
  // This method will be called once per scheduler run.
}

void Drivetrain::ArcadeDrive(double xaxisVelocity, double zaxisRotate) {
  drive.ArcadeDrive(xaxisVelocity, zaxisRotate);
}

void Drivetrain::ResetEncoders() {
  leftEncoder.Reset();
  rightEncoder.Reset();
}

int Drivetrain::GetLeftEncoderCount() {
  return leftEncoder.Get();
}

int Drivetrain::GetRightEncoderCount() {
  return rightEncoder.Get();
}

wpi::units::meters<> Drivetrain::GetLeftDistance() {
  return wpi::units::meters<>{leftEncoder.GetDistance()};
}

wpi::units::meters<> Drivetrain::GetRightDistance() {
  return wpi::units::meters<>{rightEncoder.GetDistance()};
}

wpi::units::meters<> Drivetrain::GetAverageDistance() {
  return (GetLeftDistance() + GetRightDistance()) / 2.0;
}

wpi::units::radians<> Drivetrain::GetGyroAngleX() {
  return gyro.GetAngleX();
}

wpi::units::radians<> Drivetrain::GetGyroAngleY() {
  return gyro.GetAngleY();
}

wpi::units::radians<> Drivetrain::GetGyroAngleZ() {
  return gyro.GetAngleZ();
}

void Drivetrain::ResetGyro() {
  gyro.Reset();
}
