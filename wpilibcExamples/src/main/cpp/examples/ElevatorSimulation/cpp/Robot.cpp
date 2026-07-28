// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "Constants.hpp"

void Robot::RobotPeriodic() {
  // Update the telemetry, including mechanism visualization, regardless of
  // mode.
  elevator.UpdateTelemetry();
}

void Robot::SimulationPeriodic() {
  // Update the simulation model.
  elevator.SimulationPeriodic();
}

void Robot::TeleopPeriodic() {
  if (joystick.GetTrigger()) {
    // Here, we set the constant setpoint of 0.75 meters.
    elevator.ReachGoal(Constants::kSetpoint);
  } else {
    // Otherwise, we update the setpoint to 0.
    elevator.ReachGoal(0.0_m);
  }
}

void Robot::DisabledInit() {
  // This just makes sure that our simulation code knows that the motor's off.
  elevator.Stop();
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
