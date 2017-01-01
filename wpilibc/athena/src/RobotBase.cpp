/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RobotBase.h"

#include <cstdio>

#include "DriverStation.h"
#include "HAL/HAL.h"
#include "HLUsageReporting.h"
#include "Internal/HardwareHLReporting.h"
#include "RobotState.h"
#include "SmartDashboard/SmartDashboard.h"
#include "Utility.h"
#include "WPILibVersion.h"
#include "networktables/NetworkTable.h"

using namespace frc;

std::thread::id RobotBase::m_threadId;

/**
 * Constructor for a generic robot program.
 *
 * User code should be placed in the constructor that runs before the Autonomous
 * or Operator Control period starts. The constructor will run to completion
 * before Autonomous is entered.
 *
 * This must be used to ensure that the communications code starts. In the
 * future it would be nice to put this code into it's own task that loads on
 * boot so ensure that it runs.
 */
RobotBase::RobotBase() : m_ds(DriverStation::GetInstance()) {
  m_threadId = std::this_thread::get_id();

  RobotState::SetImplementation(DriverStation::GetInstance());
  HLUsageReporting::SetImplementation(new HardwareHLReporting());

  NetworkTable::SetNetworkIdentity("Robot");
  NetworkTable::SetPersistentFilename("/home/lvuser/networktables.ini");

  SmartDashboard::init();

  std::FILE* file = nullptr;
  file = std::fopen("/tmp/frc_versions/FRC_Lib_Version.ini", "w");

  if (file != nullptr) {
    std::fputs("C++ ", file);
    std::fputs(WPILibVersion, file);
    std::fclose(file);
  }
}

/**
 * Determine if the Robot is currently enabled.
 * @return True if the Robot is currently enabled by the field controls.
 */
bool RobotBase::IsEnabled() const { return m_ds.IsEnabled(); }

/**
 * Determine if the Robot is currently disabled.
 * @return True if the Robot is currently disabled by the field controls.
 */
bool RobotBase::IsDisabled() const { return m_ds.IsDisabled(); }

/**
 * Determine if the robot is currently in Autonomous mode.
 * @return True if the robot is currently operating Autonomously as determined
 * by the field controls.
 */
bool RobotBase::IsAutonomous() const { return m_ds.IsAutonomous(); }

/**
 * Determine if the robot is currently in Operator Control mode.
 * @return True if the robot is currently operating in Tele-Op mode as
 * determined by the field controls.
 */
bool RobotBase::IsOperatorControl() const { return m_ds.IsOperatorControl(); }

/**
 * Determine if the robot is currently in Test mode.
 * @return True if the robot is currently running tests as determined by the
 * field controls.
 */
bool RobotBase::IsTest() const { return m_ds.IsTest(); }

/**
 * Indicates if new data is available from the driver station.
 * @return Has new data arrived over the network since the last time this
 * function was called?
 */
bool RobotBase::IsNewDataAvailable() const { return m_ds.IsNewControlData(); }

/**
 * Gets the ID of the main robot thread
 */
std::thread::id RobotBase::GetThreadId() { return m_threadId; }
