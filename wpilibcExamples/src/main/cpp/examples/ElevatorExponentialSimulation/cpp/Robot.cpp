// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include "Constants.h"

void Robot::RobotPeriodic() {
  // Update the telemetry, including mechanism visualization, regardless of
  // mode.
  m_elevator.UpdateTelemetry();
}

void Robot::SimulationPeriodic() {
  // Update the simulation model.
  m_elevator.SimulationPeriodic();
}

void Robot::TeleopInit() {
  // This just makes sure that our simulation code knows that the motor's off.
  m_elevator.Reset();
}

void Robot::TeleopPeriodic() {
  if (m_joystick.GetTrigger()) {
    // Here, we set the constant setpoint of 0.75 meters.
    m_elevator.ReachGoal(Constants::SETPOINT);
  } else {
    // Otherwise, we update the setpoint to 0.
    m_elevator.ReachGoal(Constants::LOWER_SETPOINT);
  }
}

void Robot::DisabledInit() {
  // This just makes sure that our simulation code knows that the motor's off.
  m_elevator.Stop();
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
