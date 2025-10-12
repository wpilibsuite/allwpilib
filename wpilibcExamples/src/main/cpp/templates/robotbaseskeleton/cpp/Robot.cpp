// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <frc/DriverStation.h>
#include <frc/internal/DriverStationModeThread.h>
#include <hal/DriverStation.h>
#include <hal/DriverStationTypes.h>
#include <networktables/NetworkTable.h>

Robot::Robot() {}

void Robot::Disabled() {}

void Robot::Autonomous() {}

void Robot::Teleop() {}

void Robot::Test() {}

void Robot::StartCompetition() {
  HAL_ControlWord word;
  HAL_GetControlWord(&word);
  frc::internal::DriverStationModeThread modeThread{word};

  // Create an opmode per robot mode
  frc::DriverStation::AddOpMode(frc::RobotMode::AUTONOMOUS, "Auto");
  frc::DriverStation::AddOpMode(frc::RobotMode::TELEOPERATED, "Teleop");
  frc::DriverStation::AddOpMode(frc::RobotMode::TEST, "Test");
  frc::DriverStation::PublishOpModes();

  wpi::Event event{false, false};
  frc::DriverStation::ProvideRefreshedDataEventHandle(event.GetHandle());

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  while (!m_exit) {
    HAL_GetControlWord(&word);
    bool enabled = IsEnabled();
    modeThread.InControl(word);
    if (!enabled) {
      Disabled();
      while (IsDisabled()) {
        wpi::WaitForObject(event.GetHandle());
      }
    } else if (IsAutonomous()) {
      Autonomous();
      while (IsAutonomousEnabled()) {
        wpi::WaitForObject(event.GetHandle());
      }
    } else if (IsTest()) {
      Test();
      while (IsTest() && IsEnabled()) {
        wpi::WaitForObject(event.GetHandle());
      }
    } else {
      Teleop();
      while (IsTeleopEnabled()) {
        wpi::WaitForObject(event.GetHandle());
      }
    }
  }
}

void Robot::EndCompetition() {
  m_exit = true;
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif
