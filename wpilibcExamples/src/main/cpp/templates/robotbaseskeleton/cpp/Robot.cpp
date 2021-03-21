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
  auto& lw = frc::LiveWindow::GetInstance();

  RobotInit();

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  while (!m_exit) {
    if (IsDisabled()) {
      m_ds.InDisabled(true);
      Disabled();
      m_ds.InDisabled(false);
      while (IsDisabled()) {
        m_ds.WaitForData();
      }
    } else if (IsAutonomous()) {
      m_ds.InAutonomous(true);
      Autonomous();
      m_ds.InAutonomous(false);
      while (IsAutonomousEnabled()) {
        m_ds.WaitForData();
      }
    } else if (IsTest()) {
      lw.SetEnabled(true);
      frc::Shuffleboard::EnableActuatorWidgets();
      m_ds.InTest(true);
      Test();
      m_ds.InTest(false);
      while (IsTest() && IsEnabled()) {
        m_ds.WaitForData();
      }
      lw.SetEnabled(false);
      frc::Shuffleboard::DisableActuatorWidgets();
    } else {
      m_ds.InOperatorControl(true);
      Teleop();
      m_ds.InOperatorControl(false);
      while (IsOperatorControlEnabled()) {
        m_ds.WaitForData();
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
