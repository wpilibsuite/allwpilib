/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/Elevator.h"

#include <chrono>

#include <frc/DriverStation.h>

constexpr double kPi = 3.14159265358979323846;

Elevator::Elevator() {
  // Set radians of encoder shaft per encoder pulse using pulses per revolution
  m_encoder.SetDistancePerPulse(2.0 * kPi / 360.0);
}

void Elevator::Enable() {
  m_elevator.Enable();
  m_thread.StartPeriodic(5_ms);
}

void Elevator::Disable() {
  m_elevator.Disable();
  m_thread.Stop();
}

void Elevator::SetReferences(double position, double velocity) {
  m_elevator.SetReferences(position, velocity);
}

void Elevator::Iterate() {
  m_elevator.SetMeasuredPosition(m_encoder.GetDistance());

  // Run controller update
  auto now = std::chrono::steady_clock::now();
  if (m_lastTime != std::chrono::steady_clock::time_point::min()) {
    m_elevator.Update(now - m_lastTime);
  } else {
    m_elevator.Update();
  }

  // Set motor input
  double batteryVoltage = frc::DriverStation::GetInstance().GetBatteryVoltage();
  m_motor.Set(m_elevator.ControllerVoltage() / batteryVoltage);

  m_lastTime = now;
}

double Elevator::ControllerVoltage() const {
  return m_elevator.ControllerVoltage();
}

void Elevator::Reset() {
  m_elevator.Reset();
  m_lastTime = std::chrono::steady_clock::time_point::min();
}
