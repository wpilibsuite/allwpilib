/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "IterativeRobot.h"

#include "DriverStation.h"
#include "HAL/HAL.h"

using namespace frc;

IterativeRobot::IterativeRobot() {
  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Iterative);
}

/**
 * Provide an alternate "main loop" via StartCompetition().
 *
 * This specific StartCompetition() implements "main loop" behaviour synced with
 * the DS packets.
 */
void IterativeRobot::StartCompetition() {
  // Loop forever, calling the appropriate mode-dependent function
  while (true) {
    // wait for driver station data so the loop doesn't hog the CPU
    DriverStation::GetInstance().WaitForData();

    LoopFunc();
  }
}
