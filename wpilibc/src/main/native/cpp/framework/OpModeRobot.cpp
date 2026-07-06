// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/OpModeRobot.hpp"

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "wpi/driverstation/RobotState.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/Notifier.hpp"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/opmode/OpMode.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/util/SafeThread.hpp"

using namespace wpi;

OpModeRobotBase::OpModeRobotBase(wpi::units::second_t period)
    : m_period{period},
      m_loopOverrunAlert{
          fmt::format("Loop time of {:.6f}s overrun", m_period.value()),
          Alert::Level::MEDIUM},
      m_watchdog{period, [this] { m_loopOverrunAlert.Set(true); }} {
  // Create our own notifier and callback queue
  int32_t status = 0;
  m_notifier = HAL_CreateNotifier(&status);
  HAL_SetNotifierName(m_notifier, "OpModeRobot", &status);

  m_startTime = std::chrono::microseconds{RobotController::GetMonotonicTime()};

  // Add LoopFunc as periodic callback
  AddPeriodic([this] { LoopFunc(); }, period);

  HAL_ReportUsage("Framework", "OpModeRobot");
}

OpModeRobotBase::OpModeRobotBase() : OpModeRobotBase(DEFAULT_PERIOD) {}

void OpModeRobotBase::AddPeriodic(std::function<void()> callback,
                                  wpi::units::second_t period) {
  m_callbacks.Add(std::move(callback), m_startTime, period);
}

void OpModeRobotBase::LoopFunc() {
  wpi::internal::DriverStationBackend::RefreshData();

  // Get current enabled state and opmode
  const hal::ControlWord word =
      wpi::internal::DriverStationBackend::GetControlWord();
  m_watchdog.Reset();
  const bool enabled = word.IsEnabled();
  int64_t modeId = word.IsDSAttached() ? word.GetOpModeId() : 0;

  bool modeChanged = modeId != m_lastModeId;
  m_lastModeId = modeId;

  if (!m_calledDriverStationConnected && word.IsDSAttached()) {
    m_calledDriverStationConnected = true;
    DriverStationConnected();
    m_watchdog.AddEpoch("DriverStationConnected()");
  }

  // Handle OpMode changes
  if (modeChanged && m_currentOpMode) {
    EndCurrentOpMode();
  }

  // Set up new opmode
  bool justCreatedOpMode = false;
  if (modeId != 0 && !m_currentOpMode && modeChanged) {
    auto data = m_opModes.lookup(modeId);
    if (data.factory) {
      // Instantiate the new opmode
      m_currentOpModeName = data.name;
      fmt::print("********** Creating OpMode {} **********\n",
                 m_currentOpModeName);
      m_currentOpMode = data.factory();
      if (m_currentOpMode) {
        // Register the opmode's additional periodic callbacks immediately on
        // creation
        m_activeOpModeCallbacks = m_currentOpMode->GetCallbacks();
        for (auto& cb : m_activeOpModeCallbacks) {
          m_callbacks.Add(cb);
        }

        // Call DisabledPeriodic immediately for newly created OpMode
        m_currentOpMode->DisabledPeriodic();
        m_watchdog.AddEpoch("OpMode::DisabledPeriodic()");
        justCreatedOpMode = true;
      }
    } else {
      WPILIB_ReportError(err::Error, "No OpMode found for mode {}", modeId);
    }
  }

  // Handle enabled state changes
  bool justCalledDisabledInit = false;
  if (m_lastEnabledState != enabled) {
    if (enabled) {
      // Transitioning to enabled
      DisabledExit();
      m_watchdog.AddEpoch("DisabledExit()");
    } else {
      // Transitioning to disabled. Only tear down an opmode that was actually
      // running; a freshly selected opmode entering its disabled phase must
      // persist so it can be started on the next enable.
      if (m_currentOpMode && m_opmodePeriodic) {
        EndCurrentOpMode();
        m_lastModeId = -1;  // force recreate next loop
      }
      DisabledInit();
      m_watchdog.AddEpoch("DisabledInit()");
      justCalledDisabledInit = true;
    }
    m_lastEnabledState = enabled;
  }

  // Start the opmode if enabled and not already started. This single check
  // covers both the disabled->enabled transition and an opmode constructed
  // while the robot is already enabled.
  if (enabled && m_currentOpMode && !m_opmodePeriodic) {
    StartCurrentOpMode();
  }

  // Call periodic functions based on current state
  if (!enabled) {
    // Only call DisabledPeriodic if we didn't just call DisabledInit
    if (!justCalledDisabledInit) {
      DisabledPeriodic();
      m_watchdog.AddEpoch("DisabledPeriodic()");
    }

    // Call opmode DisabledPeriodic if we have one
    if (m_currentOpMode && !justCreatedOpMode) {
      m_currentOpMode->DisabledPeriodic();
      m_watchdog.AddEpoch("OpMode::DisabledPeriodic()");
    }
  }

  // Call NonePeriodic when no opmode is selected
  if (modeId == 0) {
    NonePeriodic();
    m_watchdog.AddEpoch("NonePeriodic()");
  }

  // Always call RobotPeriodic
  RobotPeriodic();
  m_watchdog.AddEpoch("RobotPeriodic()");

  // Always observe user program state
  HAL_ObserveUserProgram(word.GetValue());

  SmartDashboard::UpdateValues();
  m_watchdog.AddEpoch("SmartDashboard::UpdateValues()");

  if constexpr (IsSimulation()) {
    HAL_SimPeriodicBefore();
    SimulationPeriodic();
    HAL_SimPeriodicAfter();
    m_watchdog.AddEpoch("SimulationPeriodic()");
  }

  m_watchdog.Disable();

  // Flush NetworkTables
  wpi::nt::NetworkTableInstance::GetDefault().FlushLocal();

  // Warn on loop time overruns
  if (m_watchdog.IsExpired()) {
    m_watchdog.PrintEpochs();
  }
}

void OpModeRobotBase::StartCompetition() {
  fmt::print("********** Robot program startup complete **********\n");

  if constexpr (IsSimulation()) {
    SimulationInit();
  }

  // Tell the DS that the robot is ready to be enabled
  wpi::internal::DriverStationBackend::ObserveUserProgramStarting();

  // Loop forever, calling the callback system which handles periodic functions
  while (true) {
    if (!m_callbacks.RunCallbacks(m_notifier)) {
      break;
    }
  }
}

void OpModeRobotBase::EndCompetition() {
  if (m_notifier != HAL_INVALID_HANDLE) {
    HAL_DestroyNotifier(m_notifier);
  }
}

void OpModeRobotBase::AddOpModeFactory(
    OpModeFactory factory, RobotMode mode, std::string_view name,
    std::string_view group, std::string_view description,
    const wpi::util::Color& textColor,
    const wpi::util::Color& backgroundColor) {
  int64_t id = RobotState::AddOpMode(mode, name, group, description, textColor,
                                     backgroundColor);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                                       std::string_view name,
                                       std::string_view group,
                                       std::string_view description) {
  int64_t id = RobotState::AddOpMode(mode, name, group, description);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::RemoveOpMode(RobotMode mode, std::string_view name) {
  int64_t id = RobotState::RemoveOpMode(mode, name);
  if (id != 0) {
    m_opModes.erase(id);
  }
}

void OpModeRobotBase::PublishOpModes() {
  RobotState::PublishOpModes();
}

void OpModeRobotBase::ClearOpModes() {
  RobotState::ClearOpModes();
  m_opModes.clear();
}

void OpModeRobotBase::StartCurrentOpMode() {
  if (!m_currentOpMode || m_opmodePeriodic) {
    return;
  }

  fmt::print("********** Starting OpMode {} **********\n", m_currentOpModeName);

  // Register the main opmode periodic callback. Capture a weak_ptr so a queued
  // callback can never resurrect or outlive a destroyed opmode.
  m_opmodePeriodic = wpi::internal::PeriodicPriorityQueue::Callback{
      [op = std::weak_ptr<OpMode>{m_currentOpMode}] {
        if (auto shared_op = op.lock()) {
          shared_op->Periodic();
        }
      },
      m_startTime, m_period};
  m_callbacks.Add(*m_opmodePeriodic);

  m_currentOpMode->Start();
  m_watchdog.AddEpoch("OpMode::Start()");
}

void OpModeRobotBase::EndCurrentOpMode() {
  if (!m_currentOpMode) {
    return;
  }

  // If the opmode was started, end it and remove its main periodic callback.
  if (m_opmodePeriodic) {
    fmt::print("********** Ending OpMode {} **********\n", m_currentOpModeName);

    m_currentOpMode->End();
    m_watchdog.AddEpoch("OpMode::End()");

    m_callbacks.Remove(*m_opmodePeriodic);
    m_opmodePeriodic.reset();
  }

  // The additional GetCallbacks() callbacks are registered immediately on
  // construction (even while disabled), so always remove them regardless of
  // whether the opmode was started.
  for (auto& cb : m_activeOpModeCallbacks) {
    m_callbacks.Remove(cb);
  }
  m_activeOpModeCallbacks.clear();

  // Regardless of whether opmode was started, destroy it
  fmt::print("********** Closing OpMode {} **********\n", m_currentOpModeName);
  m_currentOpMode.reset();
}
