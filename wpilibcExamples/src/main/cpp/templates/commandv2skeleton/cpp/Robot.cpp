// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/Commands.hpp"

Robot::Robot() {}

void Robot::RobotPeriodic() {
  wpi::cmd::CommandScheduler::GetInstance().Run();
}

void Robot::DisabledStart() {}

void Robot::DisabledPeriodic() {}

void Robot::DisabledEnd() {}

void Robot::AutonomousStart() {
  autonomousCommand = wpi::cmd::Print("No autonomous command configured");
  wpi::cmd::CommandScheduler::GetInstance().Schedule(autonomousCommand.value());
}

void Robot::AutonomousPeriodic() {}

void Robot::AutonomousEnd() {
  if (autonomousCommand) {
    autonomousCommand->Cancel();
  }
}

void Robot::TeleopStart() {}

void Robot::TeleopPeriodic() {}

void Robot::TeleopEnd() {}

void Robot::UtilityStart() {
  wpi::cmd::CommandScheduler::GetInstance().CancelAll();
}

void Robot::UtilityPeriodic() {}

void Robot::UtilityEnd() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
