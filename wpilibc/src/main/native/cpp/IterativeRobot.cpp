/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/IterativeRobot.h"

#include <hal/HAL.h>

#include "frc/DriverStation.h"

using namespace frc;

static constexpr double kPacketPeriod = 0.02;

IterativeRobot::IterativeRobot() : IterativeRobotBase(kPacketPeriod) {
  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Iterative);
}

void IterativeRobot::StartCompetition() {
  RobotInit();

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  // Loop forever, calling the appropriate mode-dependent function
  while (true) {
    // Wait for driver station data so the loop doesn't hog the CPU
    DriverStation::GetInstance().WaitForData();

    LoopFunc();
  }
}
