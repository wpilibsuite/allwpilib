// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

Robot::Robot() {}
void Robot::RobotPeriodic() {}

void Robot::AutonomousStart() {}
void Robot::AutonomousPeriodic() {}
void Robot::AutonomousEnd() {}

void Robot::TeleopStart() {}
void Robot::TeleopPeriodic() {}
void Robot::TeleopEnd() {}

void Robot::DisabledStart() {}
void Robot::DisabledPeriodic() {}
void Robot::DisabledEnd() {}

void Robot::UtilityStart() {}
void Robot::UtilityPeriodic() {}
void Robot::UtilityEnd() {}

void Robot::SimulationStart() {}
void Robot::SimulationPeriodic() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
