/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/Flywheel.h"

#include <chrono>

#include <frc/DriverStation.h>

constexpr double kPi = 3.14159265358979323846;

Flywheel::Flywheel() {
  // Set radians of encoder shaft per encoder pulse using pulses per revolution
  m_encoder.SetDistancePerPulse(2.0 * kPi / 360.0);
}

void Flywheel::Enable() {
  m_wheel.Enable();
  m_thread.StartPeriodic(5_ms);
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

  // Run controller update
  auto now = std::chrono::steady_clock::now();
  if (m_lastTime != std::chrono::steady_clock::time_point::min()) {
    m_wheel.Update(now - m_lastTime);
  } else {
    m_wheel.Update();
  }

  // Set motor input
  double batteryVoltage = frc::DriverStation::GetInstance().GetBatteryVoltage();
  m_motor.Set(m_wheel.ControllerVoltage() / batteryVoltage);

  m_lastTime = now;
}

void Flywheel::Reset() {
  m_wheel.Reset();
  m_lastTime = std::chrono::steady_clock::time_point::min();
}
