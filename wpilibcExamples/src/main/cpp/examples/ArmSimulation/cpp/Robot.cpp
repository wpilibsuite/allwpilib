// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

void Robot::SimulationPeriodic() {
  arm.SimulationPeriodic();
}

void Robot::TeleopInit() {
  arm.LoadPreferences();
}

void Robot::TeleopPeriodic() {
  if (joystick.GetTrigger()) {
    // Here, we run PID control like normal.
    arm.ReachSetpoint();
  } else {
    // Otherwise, we disable the motor.
    arm.Stop();
  }
}

void Robot::DisabledInit() {
  // This just makes sure that our simulation code knows that the motor's off.
  arm.Stop();
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
