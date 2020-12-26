// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
    if (m_exit)
      break;

    LoopFunc();
  }
}

void IterativeRobot::EndCompetition() {
  m_exit = true;
  DriverStation::GetInstance().WakeupWaitForData();
}
