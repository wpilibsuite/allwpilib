// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/TimedRobot.h"

#include <stdint.h>

#include <cstdio>
#include <utility>

#include <hal/DriverStation.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Notifier.h>

#include "frc/Errors.h"

using namespace frc;

void TimedRobot::StartCompetition() {
  RobotInit();

  if constexpr (IsSimulation()) {
    SimulationInit();
  }

  // Tell the DS that the robot is ready to be enabled
  std::puts("\n********** Robot program startup complete **********");
  HAL_ObserveUserProgramStarting();

  // Loop forever, calling the appropriate mode-dependent function
  while (true) {
    // We don't have to check there's an element in the queue first because
    // there's always at least one (the constructor adds one). It's reenqueued
    // at the end of the loop.
    auto callback = m_callbacks.pop();

    int32_t status = 0;
    HAL_UpdateNotifierAlarm(m_notifier, callback.expirationTime.count(),
                            &status);
    FRC_CheckErrorStatus(status, "UpdateNotifierAlarm");

    std::chrono::microseconds currentTime{
        HAL_WaitForNotifierAlarm(m_notifier, &status)};
    if (currentTime.count() == 0 || status != 0) {
      break;
    }

    callback.func();

    // Increment the expiration time by the number of full periods it's behind
    // plus one to avoid rapid repeat fires from a large loop overrun. We assume
    // currentTime ≥ expirationTime rather than checking for it since the
    // callback wouldn't be running otherwise.
    callback.expirationTime +=
        callback.period + (currentTime - callback.expirationTime) /
                              callback.period * callback.period;
    m_callbacks.push(std::move(callback));

    // Process all other callbacks that are ready to run
    while (m_callbacks.top().expirationTime <= currentTime) {
      callback = m_callbacks.pop();

      callback.func();

      callback.expirationTime +=
          callback.period + (currentTime - callback.expirationTime) /
                                callback.period * callback.period;
      m_callbacks.push(std::move(callback));
    }
  }
}

void TimedRobot::EndCompetition() {
  int32_t status = 0;
  HAL_StopNotifier(m_notifier, &status);
}

TimedRobot::TimedRobot(units::second_t period) : IterativeRobotBase(period) {
  m_startTime = std::chrono::microseconds{RobotController::GetFPGATime()};
  AddPeriodic([=, this] { LoopFunc(); }, period);

  int32_t status = 0;
  m_notifier = HAL_InitializeNotifier(&status);
  FRC_CheckErrorStatus(status, "InitializeNotifier");
  HAL_SetNotifierName(m_notifier, "TimedRobot", &status);

  HAL_Report(HALUsageReporting::kResourceType_Framework,
             HALUsageReporting::kFramework_Timed);
}

TimedRobot::~TimedRobot() {
  if (m_notifier != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_StopNotifier(m_notifier, &status);
    FRC_ReportError(status, "StopNotifier");
  }
}

void TimedRobot::AddPeriodic(std::function<void()> callback,
                             units::second_t period, units::second_t offset) {
  m_callbacks.emplace(
      callback, m_startTime,
      std::chrono::microseconds{static_cast<int64_t>(period.value() * 1e6)},
      std::chrono::microseconds{static_cast<int64_t>(offset.value() * 1e6)});
}
