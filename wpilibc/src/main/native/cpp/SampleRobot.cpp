/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/SampleRobot.h"

#include <hal/DriverStation.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <networktables/NetworkTable.h>
#include <wpi/raw_ostream.h>

#include "frc/DriverStation.h"
#include "frc/Timer.h"
#include "frc/livewindow/LiveWindow.h"

using namespace frc;

void SampleRobot::StartCompetition() {
  LiveWindow* lw = LiveWindow::GetInstance();

  RobotInit();

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  RobotMain();

  if (!m_robotMainOverridden) {
    while (true) {
      if (IsDisabled()) {
        m_ds.InDisabled(true);
        Disabled();
        m_ds.InDisabled(false);
        while (IsDisabled()) m_ds.WaitForData();
      } else if (IsAutonomous()) {
        m_ds.InAutonomous(true);
        Autonomous();
        m_ds.InAutonomous(false);
        while (IsAutonomous() && IsEnabled()) m_ds.WaitForData();
      } else if (IsTest()) {
        lw->SetEnabled(true);
        m_ds.InTest(true);
        Test();
        m_ds.InTest(false);
        while (IsTest() && IsEnabled()) m_ds.WaitForData();
        lw->SetEnabled(false);
      } else {
        m_ds.InOperatorControl(true);
        OperatorControl();
        m_ds.InOperatorControl(false);
        while (IsOperatorControl() && IsEnabled()) m_ds.WaitForData();
      }
    }
  }
}

void SampleRobot::RobotInit() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void SampleRobot::Disabled() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void SampleRobot::Autonomous() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void SampleRobot::OperatorControl() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void SampleRobot::Test() {
  wpi::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

void SampleRobot::RobotMain() { m_robotMainOverridden = false; }

SampleRobot::SampleRobot() {
  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Simple);
}
