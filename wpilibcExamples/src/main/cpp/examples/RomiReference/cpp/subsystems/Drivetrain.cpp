// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/Drivetrain.h"

using namespace DriveConstants;

// The Romi has the left and right motors set to
// PWM channels 0 and 1 respectively
// The Romi has onboard encoders that are hardcoded
// to use DIO pins 4/5 and 6/7 for the left and right
Drivetrain::Drivetrain()
    : m_left{0}, m_right{1}, m_leftEncoder{4, 5}, m_rightEncoder{6, 7} {
  ResetEncoders();
}

void Drivetrain::Periodic() {
  // Implementation of subsystem periodic method goes here.
}

void Drivetrain::ArcadeDrive(double xaxisSpeed, double zaxisRotate) {
  m_drive.ArcadeDrive(xaxisSpeed, zaxisRotate);
}

void Drivetrain::ResetEncoders() {
  m_leftEncoder.Reset();
  m_rightEncoder.Reset();
}

int Drivetrain::GetLeftEncoderCount() { return m_leftEncoder.Get(); }

int Drivetrain::GetRightEncoderCount() { return m_rightEncoder.Get(); }

units::meter_t Drivetrain::GetLeftDistance() {
  return wpi::math::pi * kWheelDiameterInch *
         (GetLeftEncoderCount() / kCountsPerRevolution);
}

units::meter_t Drivetrain::GetRightDistance() {
  return wpi::math::pi * kWheelDiameterInch *
         (GetRightEncoderCount() / kCountsPerRevolution);
}
