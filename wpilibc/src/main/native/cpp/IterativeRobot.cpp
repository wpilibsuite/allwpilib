/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/IterativeRobot.h"

#include <hal/DriverStation.h>
#include <hal/FRCUsageReporting.h>

#include "frc/DriverStation.h"

using namespace frc;

static constexpr auto kPacketPeriod = 0.02_s;

IterativeRobot::IterativeRobot() : IterativeRobotBase(kPacketPeriod) {
  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Iterative);
}

void IterativeRobot::StartCompetition() {
  RobotInit();

  if constexpr (IsSimulation()) {
    SimulationInit();
  }

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  // Loop forever, calling the appropriate mode-dependent function
  while (true) {
    // Wait for driver station data so the loop doesn't hog the CPU
    DriverStation::GetInstance().WaitForData();
    if (m_exit) break;

    LoopFunc();
  }
}

void IterativeRobot::EndCompetition() {
  m_exit = true;
  DriverStation::GetInstance().WakeupWaitForData();
}
