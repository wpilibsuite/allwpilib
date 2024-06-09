// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <frc/DriverStation.h>
#include <frc/internal/DriverStationModeThread.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <hal/DriverStation.h>
#include <networktables/NetworkTable.h>

Robot::Robot() {}

void Robot::Disabled() {}

void Robot::Autonomous() {}

void Robot::Teleop() {}

void Robot::Test() {}

void Robot::StartCompetition() {
  frc::internal::DriverStationModeThread modeThread;

  wpi::Event event{false, false};
  frc::DriverStation::ProvideRefreshedDataEventHandle(event.GetHandle());

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  while (!m_exit) {
    if (IsDisabled()) {
      modeThread.InDisabled(true);
      Disabled();
      modeThread.InDisabled(false);
      while (IsDisabled()) {
        wpi::WaitForObject(event.GetHandle());
      }
    } else if (IsAutonomous()) {
      modeThread.InAutonomous(true);
      Autonomous();
      modeThread.InAutonomous(false);
      while (IsAutonomousEnabled()) {
        wpi::WaitForObject(event.GetHandle());
      }
    } else if (IsTest()) {
      frc::LiveWindow::SetEnabled(true);
      frc::Shuffleboard::EnableActuatorWidgets();
      modeThread.InTest(true);
      Test();
      modeThread.InTest(false);
      while (IsTest() && IsEnabled()) {
        wpi::WaitForObject(event.GetHandle());
      }
      frc::LiveWindow::SetEnabled(false);
      frc::Shuffleboard::DisableActuatorWidgets();
    } else {
      modeThread.InTeleop(true);
      Teleop();
      modeThread.InTeleop(false);
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
