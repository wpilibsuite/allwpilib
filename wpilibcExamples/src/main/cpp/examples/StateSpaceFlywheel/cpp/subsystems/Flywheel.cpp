/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/Flywheel.h"

#include <frc/DriverStation.h>

constexpr double kPi = 3.14159265358979323846;

Flywheel::Flywheel() {
  // Set radians of encoder shaft per encoder pulse using pulses per revolution
  m_encoder.SetDistancePerPulse(2.0 * kPi / 360.0);
}

void Flywheel::Enable() {
  m_wheel.Enable();
  m_thread.StartPeriodic(0.005);
}

void Flywheel::Disable() {
  m_wheel.Disable();
  m_thread.Stop();
}

void Flywheel::SetReference(double angularVelocity) {
  m_wheel.SetVelocityReference(angularVelocity);
}

void Flywheel::Iterate() {
  m_wheel.SetMeasuredVelocity(m_encoder.GetRate());
  m_wheel.Update();

  // Set motor input
  double batteryVoltage = frc::DriverStation::GetInstance().GetBatteryVoltage();
  m_motor.Set(m_wheel.ControllerVoltage() / batteryVoltage);
}

void Flywheel::Reset() { m_wheel.Reset(); }
