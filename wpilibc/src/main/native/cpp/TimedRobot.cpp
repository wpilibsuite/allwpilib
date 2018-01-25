/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "TimedRobot.h"

#include <chrono>

#include <HAL/HAL.h>

using namespace frc;

/**
 * Provide an alternate "main loop" via StartCompetition().
 */
void TimedRobot::StartCompetition() {
  RobotInit();

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  // Loop forever, calling the appropriate mode-dependent function
  m_startLoop = true;
  m_loop->StartPeriodic(m_period);
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(24));
  }
}

/**
 * Set time period between calls to Periodic() functions.
 *
 * A timer event is queued for periodic event notification. Each time the
 * interrupt occurs, the event will be immediately requeued for the same time
 * interval.
 *
 * @param period Period in seconds.
 */
void TimedRobot::SetPeriod(double period) {
  m_period = period;

  if (m_startLoop) {
    m_loop->StartPeriodic(m_period);
  }
}

TimedRobot::TimedRobot() {
  m_loop = std::make_unique<Notifier>(&TimedRobot::LoopFunc, this);

  // HAL_Report(HALUsageReporting::kResourceType_Framework,
  //            HALUsageReporting::kFramework_Periodic);
  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Iterative);
}

TimedRobot::~TimedRobot() { m_loop->Stop(); }
