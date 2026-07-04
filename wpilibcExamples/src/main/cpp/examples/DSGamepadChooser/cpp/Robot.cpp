// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "opmode/BalanceAuto.hpp"
#include "opmode/DoNothingTeleop.hpp"
#include "opmode/ScoreAuto.hpp"

Robot::Robot() {
  AddOpMode<DoNothingTeleop>(wpi::RobotMode::TELEOPERATED, "Do Nothing Teleop");
  AddOpMode<ScoreAuto>(wpi::RobotMode::AUTONOMOUS, "Score Auto");
  AddOpMode<BalanceAuto>(wpi::RobotMode::AUTONOMOUS, "Balance Auto");
  PublishOpModes();
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
