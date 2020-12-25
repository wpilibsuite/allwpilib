/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/Drivetrain.h"

using namespace DriveConstants;


// The Romi has the left and right motors set to
// PWM channels 0 and 1 respectively
// The Romi has onboard encoders that are hardcoded
// to use DIO pins 4/5 and 6/7 for the left and right
Drivetrain::Drivetrain()
    : m_left{0},
      m_right{1},
      m_leftEncoder{4, 5},
      m_rightEncoder{6, 7}
      {
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

double Drivetrain::GetAverageEncoderDistance() {
  return (m_leftEncoder.GetDistance() + m_rightEncoder.GetDistance()) / 2.0;
}

int Drivetrain::GetLeftEncoderCount() { return m_leftEncoder.Get(); }

int Drivetrain::GetRightEncoderCount() { return m_rightEncoder.Get(); }

units::length_t Drivetrain::GetLeftDistance() {
  return wpi::math::pi * kWheelDiameterInch * (GetLeftEncoderCount() / kCountsPerRevolution);
}

units::length_t Drivetrain::GetRightDistance() {
  return wpi::math::pi * kWheelDiameterInch * (GetRightEncoderCount() / kCountsPerRevolution);
}