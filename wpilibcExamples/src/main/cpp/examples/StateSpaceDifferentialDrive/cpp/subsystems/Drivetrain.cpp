/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/Drivetrain.h"

#include <frc/DriverStation.h>

constexpr double kPi = 3.14159265358979323846;

Drivetrain::Drivetrain() {
  // Set radians of encoder shaft per encoder pulse using pulses per revolution
  m_leftEncoder.SetDistancePerPulse(2.0 * kPi / 360.0);
  m_rightEncoder.SetDistancePerPulse(2.0 * kPi / 360.0);
}

void Drivetrain::Enable() {
  m_drivetrain.Enable();
  m_thread.StartPeriodic(0.005);
}

void Drivetrain::Disable() {
  m_drivetrain.Disable();
  m_thread.Stop();
}

void Drivetrain::SetReferences(double leftPosition, double leftVelocity,
                               double rightPosition, double rightVelocity) {
  m_drivetrain.SetReferences(leftPosition, leftVelocity, rightPosition,
                             rightVelocity);
}

void Drivetrain::Iterate() {
  m_drivetrain.SetMeasuredStates(m_leftEncoder.GetDistance(),
                                 m_rightEncoder.GetDistance());
  m_drivetrain.Update();

  // Set motor inputs
  double batteryVoltage = frc::DriverStation::GetInstance().GetBatteryVoltage();
  m_left.Set(m_drivetrain.ControllerLeftVoltage() / batteryVoltage);
  m_right.Set(m_drivetrain.ControllerRightVoltage() / batteryVoltage);
}

double Drivetrain::ControllerLeftVoltage() const {
  return m_drivetrain.ControllerLeftVoltage();
}

double Drivetrain::ControllerRightVoltage() const {
  return m_drivetrain.ControllerRightVoltage();
}

void Drivetrain::Reset() { m_drivetrain.Reset(); }
