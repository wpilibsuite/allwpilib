// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/internal/DriverStationModeThread.hpp"

Robot::Robot() {}

void Robot::Disabled() {}

void Robot::Autonomous() {}

void Robot::Teleop() {}

void Robot::Test() {}

void Robot::StartCompetition() {
  wpi::internal::DriverStationModeThread modeThread{wpi::hal::GetControlWord()};

  // Create an opmode per robot mode
  wpi::DriverStation::AddOpMode(wpi::RobotMode::AUTONOMOUS, "Auto");
  wpi::DriverStation::AddOpMode(wpi::RobotMode::TELEOPERATED, "Teleop");
  wpi::DriverStation::AddOpMode(wpi::RobotMode::TEST, "Test");
  wpi::DriverStation::PublishOpModes();

  wpi::util::Event event{false, false};
  wpi::DriverStation::ProvideRefreshedDataEventHandle(event.GetHandle());

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  while (!m_exit) {
    modeThread.InControl(wpi::DriverStation::GetControlWord());
    if (IsDisabled()) {
      Disabled();
      while (IsDisabled()) {
        wpi::util::WaitForObject(event.GetHandle());
      }
    } else if (IsAutonomous()) {
      Autonomous();
      while (IsAutonomousEnabled()) {
        wpi::util::WaitForObject(event.GetHandle());
      }
    } else if (IsTest()) {
      Test();
      while (IsTest() && IsEnabled()) {
        wpi::util::WaitForObject(event.GetHandle());
      }
    } else {
      Teleop();
      while (IsTeleopEnabled()) {
        wpi::util::WaitForObject(event.GetHandle());
      }
    }
  }
}

void Robot::EndCompetition() {
  m_exit = true;
}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
