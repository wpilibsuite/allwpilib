/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "subsystems/SingleJointedArm.h"

#include <chrono>

#include <frc/DriverStation.h>

constexpr double kPi = 3.14159265358979323846;

SingleJointedArm::SingleJointedArm() {
  // Set radians of encoder shaft per encoder pulse using pulses per revolution
  m_encoder.SetDistancePerPulse(2.0 * kPi / 360.0);
}

void SingleJointedArm::Enable() {
  m_singleJointedArm.Enable();
  m_thread.StartPeriodic(5_ms);
}

void SingleJointedArm::Disable() {
  m_singleJointedArm.Disable();
  m_thread.Stop();
}

void SingleJointedArm::SetReferences(double angle, double angularVelocity) {
  m_singleJointedArm.SetReferences(angle, angularVelocity);
}

void SingleJointedArm::Iterate() {
  m_singleJointedArm.SetMeasuredAngle(m_encoder.GetDistance());

  // Run controller update
  auto now = std::chrono::steady_clock::now();
  if (m_lastTime != std::chrono::steady_clock::time_point::min()) {
    m_singleJointedArm.Update(now - m_lastTime);
  } else {
    m_singleJointedArm.Update();
  }

  // Set motor input
  double batteryVoltage = frc::DriverStation::GetInstance().GetBatteryVoltage();
  m_motor.Set(m_singleJointedArm.ControllerVoltage() / batteryVoltage);

  m_lastTime = now;
}

double SingleJointedArm::ControllerVoltage() const {
  return m_singleJointedArm.ControllerVoltage();
}

void SingleJointedArm::Reset() {
  m_singleJointedArm.Reset();
  m_lastTime = std::chrono::steady_clock::time_point::min();
}
