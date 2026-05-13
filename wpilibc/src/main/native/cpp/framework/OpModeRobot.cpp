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
  bool calledOpModeDisabledPeriodicThisIteration = false;

  if (!m_calledDriverStationConnected && word.IsDSAttached()) {
    m_calledDriverStationConnected = true;
    DriverStationConnected();
    m_watchdog.AddEpoch("DriverStationConnected()");
  }

  // Handle OpMode changes
  if (modeId != m_lastModeId && m_currentOpMode) {
    EndCurrentOpMode();
  }

  // Set up new opmode
  if (modeId != 0 && !m_currentOpMode) {
    fmt::print(
        "DEBUG: Looking for OpMode with ID {} in registry with {} entries\n",
        modeId, m_opModes.size());
    auto data = m_opModes.lookup(modeId);
    if (data.factory) {
      // Instantiate the new opmode
      fmt::print("********** Creating OpMode {} **********\n", data.name);
      m_currentOpMode = data.factory();
      if (m_currentOpMode) {
        // Register the opmode's additional periodic callbacks immediately on
        // creation
        m_activeOpModeCallbacks = m_currentOpMode->GetCallbacks();
        for (auto& cb : m_activeOpModeCallbacks) {
          m_callbacks.Add(cb);
        }

        if (!enabled) {
          // Call DisabledPeriodic immediately for newly created OpMode when
          // disabled
          m_currentOpMode->DisabledPeriodic();
          m_watchdog.AddEpoch("OpMode::DisabledPeriodic()");
          calledOpModeDisabledPeriodicThisIteration = true;
        } else {
          // If robot is enabled, start the OpMode immediately
          if (!m_opmodePeriodic) {
            fmt::print("********** Starting OpMode **********\n");
            // Register the main opmode periodic callback
            m_opmodePeriodic = wpi::internal::PeriodicPriorityQueue::Callback{
                [op = m_currentOpMode.get()] { op->Periodic(); }, m_startTime,
                m_period};
            m_callbacks.Add(*m_opmodePeriodic);

            m_currentOpMode->Start();
            m_watchdog.AddEpoch("OpMode::Start()");
          }
        }
      }
      // Update m_lastModeId immediately to prevent race conditions
      m_lastModeId = modeId;
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
      if (m_currentOpMode && !m_opmodePeriodic) {
        // Only start if not already started
        // Register the main opmode periodic callback
        m_opmodePeriodic = wpi::internal::PeriodicPriorityQueue::Callback{
            [op = m_currentOpMode.get()] { op->Periodic(); }, m_startTime,
            m_period};
        m_callbacks.Add(*m_opmodePeriodic);

        // Start the opmode
        fmt::print("********** Starting OpMode **********\n");
        m_currentOpMode->Start();
        m_watchdog.AddEpoch("OpMode::Start()");
      }
    } else {
      // Transitioning to disabled
      if (m_currentOpMode) {
        EndCurrentOpMode();
      }
      DisabledInit();
      m_watchdog.AddEpoch("DisabledInit()");
      justCalledDisabledInit = true;
    }
    m_lastEnabledState = enabled;
  }

  // Call periodic functions based on current state
  if (!enabled) {
    // Only call DisabledPeriodic if we didn't just call DisabledInit
    if (!justCalledDisabledInit) {
      DisabledPeriodic();
      m_watchdog.AddEpoch("DisabledPeriodic()");
    }

    // Call opmode DisabledPeriodic if we have one and haven't called it already
    // this iteration
    if (m_currentOpMode && !calledOpModeDisabledPeriodicThisIteration) {
      m_currentOpMode->DisabledPeriodic();
      m_watchdog.AddEpoch("OpMode::DisabledPeriodic()");
    }
  }

  // Call NonePeriodic when no opmode is selected
  if (modeId == 0) {
    m_lastModeId = modeId;
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
    fmt::print("DEBUG: Registering OpMode '{}' with ID {}\n", name, id);
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  } else {
    fmt::print(
        "DEBUG: Failed to register OpMode '{}' - RobotState::AddOpMode "
        "returned 0\n",
        name);
  }
}

void OpModeRobotBase::AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                                       std::string_view name,
                                       std::string_view group,
                                       std::string_view description) {
  int64_t id = RobotState::AddOpMode(mode, name, group, description);
  if (id != 0) {
    fmt::print("DEBUG: Registering OpMode '{}' with ID {}\n", name, id);
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  } else {
    fmt::print(
        "DEBUG: Failed to register OpMode '{}' - RobotState::AddOpMode "
        "returned 0\n",
        name);
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

void OpModeRobotBase::EndCurrentOpMode() {
  if (!m_currentOpMode) {
    return;
  }

  // If opmode was started (enabled)
  if (m_opmodePeriodic) {
    fmt::print("********** Ending OpMode **********\n");

    m_currentOpMode->End();
    m_watchdog.AddEpoch("OpMode::End()");

    // Remove opmode callbacks
    m_callbacks.Remove(*m_opmodePeriodic);
    m_opmodePeriodic.reset();
    for (auto& cb : m_activeOpModeCallbacks) {
      m_callbacks.Remove(cb);
    }
    m_activeOpModeCallbacks.clear();
  }

  // Regardless of whether opmode was started, destroy it
  fmt::print("********** Closing OpMode **********\n");
  m_currentOpMode.reset();
}
