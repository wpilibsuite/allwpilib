// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

void Robot::SimulationPeriodic() {
  m_arm.SimulationPeriodic();
}

void Robot::TeleopInit() {
  m_arm.LoadPreferences();
}

void Robot::TeleopPeriodic() {
  if (m_joystick.GetTrigger()) {
    // Here, we run PID control like normal.
    m_arm.ReachSetpoint();
  } else {
    // Otherwise, we disable the motor.
    m_arm.Stop();
  }
}

void Robot::DisabledInit() {
  // This just makes sure that our simulation code knows that the motor's off.
  m_arm.Stop();
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
