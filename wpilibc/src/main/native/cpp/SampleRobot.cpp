/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "SampleRobot.h"

#include <llvm/raw_ostream.h>
#include <networktables/NetworkTable.h>

#include "DriverStation.h"
#include "LiveWindow/LiveWindow.h"
#include "Timer.h"

using namespace frc;

/**
 * Start a competition.
 *
 * This code needs to track the order of the field starting to ensure that
 * everything happens in the right order. Repeatedly run the correct method,
 * either Autonomous or OperatorControl or Test when the robot is enabled.
 * After running the correct method, wait for some state to change, either the
 * other mode starts or the robot is disabled. Then go back and wait for the
 * robot to be enabled again.
 */
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

/**
 * Robot-wide initialization code should go here.
 *
 * Users should override this method for default Robot-wide initialization which
 * will be called when the robot is first powered on. It will be called exactly
 * one time.
 *
 * Warning: the Driver Station "Robot Code" light and FMS "Robot Ready"
 * indicators will be off until RobotInit() exits. Code in RobotInit() that
 * waits for enable will cause the robot to never indicate that the code is
 * ready, causing the robot to be bypassed in a match.
 */
void SampleRobot::RobotInit() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Disabled should go here.
 *
 * Programmers should override this method to run code that should run while the
 * field is disabled.
 */
void SampleRobot::Disabled() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Autonomous should go here.
 *
 * Programmers should override this method to run code that should run while the
 * field is in the autonomous period. This will be called once each time the
 * robot enters the autonomous state.
 */
void SampleRobot::Autonomous() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Operator control (tele-operated) code should go here.
 *
 * Programmers should override this method to run code that should run while the
 * field is in the Operator Control (tele-operated) period. This is called once
 * each time the robot enters the teleop state.
 */
void SampleRobot::OperatorControl() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Test program should go here.
 *
 * Programmers should override this method to run code that executes while the
 * robot is in test mode. This will be called once whenever the robot enters
 * test mode
 */
void SampleRobot::Test() {
  llvm::outs() << "Default " << __FUNCTION__ << "() method... Overload me!\n";
}

/**
 * Robot main program for free-form programs.
 *
 * This should be overridden by user subclasses if the intent is to not use the
 * Autonomous() and OperatorControl() methods. In that case, the program is
 * responsible for sensing when to run the autonomous and operator control
 * functions in their program.
 *
 * This method will be called immediately after the constructor is called. If it
 * has not been overridden by a user subclass (i.e. the default version runs),
 * then the Autonomous() and OperatorControl() methods will be called.
 */
void SampleRobot::RobotMain() { m_robotMainOverridden = false; }

SampleRobot::SampleRobot() {
  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Simple);
}
