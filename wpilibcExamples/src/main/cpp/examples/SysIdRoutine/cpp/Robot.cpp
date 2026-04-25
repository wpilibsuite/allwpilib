// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/commands2/CommandScheduler.hpp"

Robot::Robot() {}

void Robot::RobotPeriodic() {
  wpi::cmd::CommandScheduler::GetInstance().Run();
}

void Robot::DisabledInit() {}

void Robot::DisabledPeriodic() {}

void Robot::DisabledExit() {}

void Robot::AutonomousInit() {
  autonomousCommand = container.GetAutonomousCommand();

  if (autonomousCommand) {
    wpi::cmd::CommandScheduler::GetInstance().Schedule(
        autonomousCommand.value());
  }
}

void Robot::AutonomousPeriodic() {}

void Robot::AutonomousExit() {}

void Robot::TeleopInit() {
  if (autonomousCommand) {
    autonomousCommand->Cancel();
  }
}

void Robot::TeleopPeriodic() {}

void Robot::TeleopExit() {}

void Robot::UtilityInit() {
  wpi::cmd::CommandScheduler::GetInstance().CancelAll();
}

void Robot::UtilityPeriodic() {}

void Robot::UtilityExit() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
