// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drivetrain.h"

#include <numbers>

#include "Constants.h"

using namespace DriveConstants;

// The Romi has the left and right motors set to
// PWM channels 0 and 1 respectively
// The Romi has onboard encoders that are hardcoded
// to use DIO pins 4/5 and 6/7 for the left and right
Drivetrain::Drivetrain() {
  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightMotor.SetInverted(true);

  m_leftEncoder.SetDistancePerPulse(std::numbers::pi * kWheelDiameter.value() /
                                    kCountsPerRevolution);
  m_rightEncoder.SetDistancePerPulse(std::numbers::pi * kWheelDiameter.value() /
                                     kCountsPerRevolution);
  ResetEncoders();
}

void Drivetrain::Periodic() {
  // This method will be called once per scheduler run.
}

void Drivetrain::ArcadeDrive(double xaxisSpeed, double zaxisRotate) {
  m_drive.ArcadeDrive(xaxisSpeed, zaxisRotate);
}

void Drivetrain::ResetEncoders() {
  m_leftEncoder.Reset();
  m_rightEncoder.Reset();
}

int Drivetrain::GetLeftEncoderCount() {
  return m_leftEncoder.Get();
}

int Drivetrain::GetRightEncoderCount() {
  return m_rightEncoder.Get();
}

units::meter_t Drivetrain::GetLeftDistance() {
  return units::meter_t{m_leftEncoder.GetDistance()};
}

units::meter_t Drivetrain::GetRightDistance() {
  return units::meter_t{m_rightEncoder.GetDistance()};
}

units::meter_t Drivetrain::GetAverageDistance() {
  return (GetLeftDistance() + GetRightDistance()) / 2.0;
}

double Drivetrain::GetAccelX() {
  return m_accelerometer.GetX();
}

double Drivetrain::GetAccelY() {
  return m_accelerometer.GetY();
}

double Drivetrain::GetAccelZ() {
  return m_accelerometer.GetZ();
}

double Drivetrain::GetGyroAngleX() {
  return m_gyro.GetAngleX();
}

double Drivetrain::GetGyroAngleY() {
  return m_gyro.GetAngleY();
}

double Drivetrain::GetGyroAngleZ() {
  return m_gyro.GetAngleZ();
}

void Drivetrain::ResetGyro() {
  m_gyro.Reset();
}
