// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <frc/DriverStation.h>
#include <frc/internal/DriverStationModeThread.h>
#include <hal/DriverStation.h>
#include <networktables/NetworkTable.h>
#include "hal/DriverStationTypes.h"

Robot::Robot() {}

void Robot::Disabled() {}

void Robot::Autonomous() {}

void Robot::Teleop() {}

void Robot::Test() {}

void Robot::StartCompetition() {
  frc::internal::DriverStationModeThread modeThread;

  // Create an opmode per robot mode
  WPI_String name;
  WPI_InitString(&name, "Auto");
  HAL_AddOpMode(HAL_ROBOTMODE_AUTONOMOUS, &name, nullptr, nullptr, -1, -1);
  WPI_InitString(&name, "Teleop");
  HAL_AddOpMode(HAL_ROBOTMODE_TELEOPERATED, &name, nullptr, nullptr, -1, -1);
  WPI_InitString(&name, "Test");
  HAL_AddOpMode(HAL_ROBOTMODE_TEST, &name, nullptr, nullptr, -1, -1);

  wpi::Event event{false, false};
  frc::DriverStation::ProvideRefreshedDataEventHandle(event.GetHandle());

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  while (!m_exit) {
    int64_t opMode = GetOpModeId();
    bool enabled = IsEnabled();
    modeThread.InOpMode(opMode, enabled);
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
