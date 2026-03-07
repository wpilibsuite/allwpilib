// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"

#include <stdint.h>

#include <cstdio>
#include <utility>

#include "wpi/hal/DriverStation.hpp"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/RobotController.hpp"

using namespace wpi;

void TimedRobot::StartCompetition() {
  if constexpr (IsSimulation()) {
    SimulationInit();
  }

  // Tell the DS that the robot is ready to be enabled
  std::puts("\n********** Robot program startup complete **********");
  HAL_ObserveUserProgramStarting();

  // Loop forever, calling the appropriate mode-dependent function
  while (true) {
    m_loopStartTimeUs = RobotController::GetFPGATime();
    if (!m_callbacks.RunCallbacks(m_notifier)) {
      break;
    }
  }
}

void TimedRobot::EndCompetition() {
  HAL_DestroyNotifier(m_notifier);
  m_notifier = HAL_kInvalidHandle;
}

TimedRobot::TimedRobot(wpi::units::second_t period)
    : IterativeRobotBase(period) {
  m_startTime = std::chrono::microseconds{RobotController::GetFPGATime()};
  AddPeriodic([=, this] { LoopFunc(); }, period);

  int32_t status = 0;
  m_notifier = HAL_CreateNotifier(&status);
  WPILIB_CheckErrorStatus(status, "InitializeNotifier");
  HAL_SetNotifierName(m_notifier, "TimedRobot", &status);

  HAL_ReportUsage("Framework", "TimedRobot");
}

TimedRobot::TimedRobot(wpi::units::hertz_t frequency)
    : TimedRobot{1 / frequency} {}

TimedRobot::~TimedRobot() {
  if (m_notifier != HAL_kInvalidHandle) {
    HAL_DestroyNotifier(m_notifier);
  }
}

uint64_t TimedRobot::GetLoopStartTime() {
  return m_loopStartTimeUs;
}

void TimedRobot::AddPeriodic(std::function<void()> callback,
                             wpi::units::second_t period,
                             wpi::units::second_t offset) {
  m_callbacks.Add(std::move(callback), m_startTime, period, offset);
}
