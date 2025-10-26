// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/opmode/PeriodicOpMode.h"

#include <hal/DriverStation.h>
#include <hal/UsageReporting.h>
#include <networktables/NetworkTableInstance.h>

#include "frc/DriverStation.h"
#include "frc/Errors.h"
#include "frc/RobotController.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

PeriodicOpMode::Callback::Callback(std::function<void()> func,
                                   std::chrono::microseconds startTime,
                                   std::chrono::microseconds period,
                                   std::chrono::microseconds offset)
    : func{std::move(func)},
      period{period},
      expirationTime(
          startTime + offset + period +
          (std::chrono::microseconds{frc::RobotController::GetFPGATime()} -
           startTime) /
              period * period) {}

PeriodicOpMode::~PeriodicOpMode() {
  if (m_notifier != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_StopNotifier(m_notifier, &status);
    FRC_ReportError(status, "StopNotifier");
  }
}

PeriodicOpMode::PeriodicOpMode(units::second_t period)
    : m_period{period},
      m_watchdog(period, [this] { PrintLoopOverrunMessage(); }) {
  m_startTime = std::chrono::microseconds{RobotController::GetFPGATime()};
  AddPeriodic([=, this] { LoopFunc(); }, period);

  int32_t status = 0;
  m_notifier = HAL_InitializeNotifier(&status);
  FRC_CheckErrorStatus(status, "InitializeNotifier");
  HAL_SetNotifierName(m_notifier, "PeriodicOpMode", &status);

  HAL_ReportUsage("OpMode", "PeriodicOpMode");
}

void PeriodicOpMode::AddPeriodic(std::function<void()> callback,
                                 units::second_t period,
                                 units::second_t offset) {
  m_callbacks.emplace(
      callback, m_startTime,
      std::chrono::microseconds{static_cast<int64_t>(period.value() * 1e6)},
      std::chrono::microseconds{static_cast<int64_t>(offset.value() * 1e6)});
}

void PeriodicOpMode::LoopFunc() {
  DriverStation::RefreshData();
  HAL_ControlWord word;
  HAL_GetControlWord(&word);
  HAL_ControlWord_SetOpModeId(&word, m_opModeId);
  HAL_ObserveUserProgram(word);

  if (!DriverStation::IsEnabled() ||
      DriverStation::GetOpModeId() != m_opModeId) {
    m_running = false;
    return;
  }

  m_watchdog.Reset();
  Periodic();
  m_watchdog.AddEpoch("Periodic()");

  SmartDashboard::UpdateValues();
  m_watchdog.AddEpoch("SmartDashboard::UpdateValues()");

  // if constexpr (IsSimulation()) {
  //   HAL_SimPeriodicBefore();
  //   SimulationPeriodic();
  //   HAL_SimPeriodicAfter();
  //   m_watchdog.AddEpoch("SimulationPeriodic()");
  // }

  m_watchdog.Disable();

  // Flush NetworkTables
  nt::NetworkTableInstance::GetDefault().FlushLocal();

  // Warn on loop time overruns
  if (m_watchdog.IsExpired()) {
    m_watchdog.PrintEpochs();
  }
}

void PeriodicOpMode::OpModeRun(int64_t opModeId) {
  m_opModeId = opModeId;

  Start();
  while (m_running) {
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

    m_loopStartTimeUs = RobotController::GetFPGATime();

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
  End();
}

void PeriodicOpMode::OpModeStop() {
  int32_t status = 0;
  HAL_StopNotifier(m_notifier, &status);
}

void PeriodicOpMode::PrintLoopOverrunMessage() {
  FRC_ReportWarning("Loop time of {:.6f}s overrun", m_period.value());
}

void PeriodicOpMode::PrintWatchdogEpochs() {
  m_watchdog.PrintEpochs();
}
