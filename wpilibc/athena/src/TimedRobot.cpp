/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "TimedRobot.h"

#include <chrono>

#include "HAL/HAL.h"

/**
 * Provide an alternate "main loop" via StartCompetition().
 */
void frc::TimedRobot::StartCompetition() {
  // Loop forever, calling the appropriate mode-dependent function
  m_startLoop = true;
  m_loop->StartPeriodic(m_period);
  while (true) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(24h);
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
void frc::TimedRobot::SetPeriod(double period) {
  m_period = period;

  if (m_startLoop) {
    m_loop->StartPeriodic(m_period);
  }
}

frc::TimedRobot::TimedRobot() {
  m_loop = std::make_unique<Notifier>(&frc::TimedRobot::LoopFunc, this);

  // HAL_Report(HALUsageReporting::kResourceType_Framework,
  //            HALUsageReporting::kFramework_Periodic);
  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Iterative);
}

frc::TimedRobot::~TimedRobot() { m_loop->Stop(); }
