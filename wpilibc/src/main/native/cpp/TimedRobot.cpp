/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/TimedRobot.h"

#include <stdint.h>

#include <utility>

#include <hal/DriverStation.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Notifier.h>

#include "frc/Timer.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"

using namespace frc;

void TimedRobot::StartCompetition() {
  RobotInit();

  if constexpr (IsSimulation()) {
    SimulationInit();
  }

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

  m_expirationTime = units::second_t{Timer::GetFPGATimestamp()} + m_period;
  UpdateAlarm();

  // Loop forever, calling the appropriate mode-dependent function
  while (true) {
    int32_t status = 0;
    uint64_t curTime = HAL_WaitForNotifierAlarm(m_notifier, &status);
    if (curTime == 0 || status != 0) break;

    m_expirationTime += m_period;

    UpdateAlarm();

    // Call callback
    LoopFunc();
  }
}

void TimedRobot::EndCompetition() {
  int32_t status = 0;
  HAL_StopNotifier(m_notifier, &status);
}

units::second_t TimedRobot::GetPeriod() const {
  return units::second_t(m_period);
}

TimedRobot::TimedRobot(double period) : TimedRobot(units::second_t(period)) {}

TimedRobot::TimedRobot(units::second_t period) : IterativeRobotBase(period) {
  int32_t status = 0;
  m_notifier = HAL_InitializeNotifier(&status);
  wpi_setHALError(status);
  HAL_SetNotifierName(m_notifier, "TimedRobot", &status);

  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Timed);
}

TimedRobot::~TimedRobot() {
  int32_t status = 0;

  HAL_StopNotifier(m_notifier, &status);
  wpi_setHALError(status);

  HAL_CleanNotifier(m_notifier, &status);
}

void TimedRobot::UpdateAlarm() {
  int32_t status = 0;
  HAL_UpdateNotifierAlarm(
      m_notifier, static_cast<uint64_t>(m_expirationTime * 1e6), &status);
  wpi_setHALError(status);
}
