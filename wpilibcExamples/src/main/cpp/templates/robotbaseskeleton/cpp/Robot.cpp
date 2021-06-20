// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.h"

#include <frc/DriverStation.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <hal/DriverStation.h>
#include <networktables/NetworkTable.h>

void Robot::RobotInit() {}

void Robot::Disabled() {}

void Robot::Autonomous() {}

void Robot::Teleop() {}

void Robot::Test() {}

void Robot::StartCompetition() {
  RobotInit();

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  while (!m_exit) {
    if (IsDisabled()) {
      frc::DriverStation::InDisabled(true);
      Disabled();
      frc::DriverStation::InDisabled(false);
      while (IsDisabled()) {
        frc::DriverStation::WaitForData();
      }
    } else if (IsAutonomous()) {
      frc::DriverStation::InAutonomous(true);
      Autonomous();
      frc::DriverStation::InAutonomous(false);
      while (IsAutonomousEnabled()) {
        frc::DriverStation::WaitForData();
      }
    } else if (IsTest()) {
      frc::LiveWindow::SetEnabled(true);
      frc::Shuffleboard::EnableActuatorWidgets();
      frc::DriverStation::InTest(true);
      Test();
      frc::DriverStation::InTest(false);
      while (IsTest() && IsEnabled()) {
        frc::DriverStation::WaitForData();
      }
      frc::LiveWindow::SetEnabled(false);
      frc::Shuffleboard::DisableActuatorWidgets();
    } else {
      frc::DriverStation::InOperatorControl(true);
      Teleop();
      frc::DriverStation::InOperatorControl(false);
      while (IsOperatorControlEnabled()) {
        frc::DriverStation::WaitForData();
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
