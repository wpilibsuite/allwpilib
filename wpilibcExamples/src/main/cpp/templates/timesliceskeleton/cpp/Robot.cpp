// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <frc/livewindow/LiveWindow.h>

// Run robot periodic() functions for 5 ms, and run controllers every 10 ms
Robot::Robot() : frc::TimesliceRobot{5_ms, 10_ms} {
  // LiveWindow causes drastic overruns in robot periodic functions that will
  // interfere with controllers
  frc::LiveWindow::DisableAllTelemetry();

  // Runs for 2 ms after robot periodic functions
  Schedule([=] {}, 2_ms);

  // Runs for 2 ms after first controller function
  Schedule([=] {}, 2_ms);

  // Total usage:
  // 5 ms (robot) + 2 ms (controller 1) + 2 ms (controller 2) = 9 ms
  // 9 ms / 10 ms -> 90% allocated
}

void Robot::RobotPeriodic() {}

void Robot::AutonomousInit() {}
void Robot::AutonomousPeriodic() {}

void Robot::TeleopInit() {}
void Robot::TeleopPeriodic() {}

void Robot::DisabledInit() {}
void Robot::DisabledPeriodic() {}

void Robot::TestInit() {}
void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
