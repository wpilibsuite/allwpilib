// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/OpModeRobot.hpp"

#include <cstdio>
#include <cstdlib>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "wpi/driverstation/RobotState.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/Notifier.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/opmode/OpMode.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/UsageReporting.hpp"
#include "wpi/util/print.hpp"

using namespace wpi;

OpModeRobotBase::OpModeRobotBase(wpi::units::second_t period)
    : m_period{period},
      m_loopOverrunAlert{
          "loop-overrun",
          std::format("Loop time of {:.6f}s overrun", m_period.value()),
          wpi::util::Alert::Level::MEDIUM},
      m_watchdog{period, [this] { m_loopOverrunAlert.Set(true); }},
      m_opModePeriodicOverrunAlert{
          "opmode-periodic-overrun",
          std::format("OpMode Periodic() time of {:.6f}s overrun",
                      m_period.value()),
          wpi::util::Alert::Level::MEDIUM},
      m_opModeWatchdog{period,
                       [this] { m_opModePeriodicOverrunAlert.Set(true); }} {
  // Create our own notifier and callback queue
  int32_t status = 0;
  m_notifier = HAL_CreateNotifier(&status);
  HAL_SetNotifierName(m_notifier, "OpModeRobot", &status);

  m_startTime = std::chrono::nanoseconds{RobotController::GetMonotonicTime()};

  // Add LoopFunc as periodic callback
  AddPeriodic([this] { LoopFunc(); }, period);

  wpi::util::ReportUsage("Framework", "OpModeRobot");
}

OpModeRobotBase::OpModeRobotBase(wpi::units::hertz_t frequency)
    : OpModeRobotBase{1 / frequency} {}

OpModeRobotBase::OpModeRobotBase() : OpModeRobotBase(DEFAULT_PERIOD) {}

void OpModeRobotBase::AddPeriodic(std::function<void()> callback,
                                  wpi::units::second_t period,
                                  wpi::units::second_t offset) {
  m_callbacks.Add(std::move(callback), m_startTime, period, offset);
}

void OpModeRobotBase::PrintWatchdogEpochs() {
  m_watchdog.PrintEpochs();
}

void OpModeRobotBase::LoopFunc() {
  wpi::internal::DriverStationBackend::RefreshData();

  // Get current enabled state and opmode
  const hal::ControlWord word =
      wpi::internal::DriverStationBackend::GetControlWord();
  m_watchdog.Reset();
  const bool enabled = word.IsEnabled();
  // Treat disabled as unknown
  const RobotMode mode = enabled ? word.GetRobotMode() : RobotMode::UNKNOWN;
  int64_t modeId = word.IsDSAttached() ? word.GetOpModeId() : 0;

  bool modeChanged = modeId != m_lastModeId;
  m_lastModeId = modeId;

  if (!m_calledDriverStationConnected && word.IsDSAttached()) {
    m_calledDriverStationConnected = true;
    DriverStationConnected();
  }

  // Handle OpMode changes
  if (modeChanged && m_currentOpMode) {
    EndCurrentOpMode();
  }

  // Set up new opmode
  if (modeId != 0 && !m_currentOpMode && modeChanged) {
    auto data = m_opModes.lookup(modeId);
    if (data.factory) {
      // Instantiate the new opmode
      m_currentOpModeName = data.name;
      wpi::util::print("********** Creating OpMode {} **********\n",
                       m_currentOpModeName);
      m_currentOpMode = data.factory();
      if (m_currentOpMode) {
        // Register the opmode's additional periodic callbacks immediately on
        // creation
        m_activeOpModeCallbacks = m_currentOpMode->GetCallbacks();
        for (auto& cb : m_activeOpModeCallbacks) {
          m_callbacks.Add(cb);
        }

        // An opmode created while enabled still gets one DisabledPeriodic call
        // before starting, as it may contain setup code. When disabled, it is
        // called below with the rest of the disabled periodic functions.
        if (enabled) {
          m_currentOpMode->DisabledPeriodic();
          m_watchdog.AddEpoch("OpMode::DisabledPeriodic()");
        }
      }
    } else {
      WPILIB_ReportError(err::Error, "No OpMode found for mode {}", modeId);
    }
  }

  // If mode changed, call disabled exit and entry functions
  if (m_lastMode != mode) {
    if (m_lastMode == RobotMode::UNKNOWN) {
      // Transitioning out of disabled
      DisabledExit();
    }
    if (mode == RobotMode::UNKNOWN) {
      // Transitioning to disabled. Only tear down an opmode that was actually
      // running; a freshly selected opmode entering its disabled phase must
      // persist so it can be started on the next enable.
      if (m_currentOpMode && m_opmodePeriodic) {
        EndCurrentOpMode();
        m_lastModeId = -1;  // force recreate next loop
      }
      DisabledInit();
      m_watchdog.AddEpoch("DisabledInit()");
    }
    m_lastMode = mode;
  }

  // Start the opmode if enabled and not already started. This single check
  // covers both the disabled->enabled transition and an opmode constructed
  // while the robot is already enabled.
  if (enabled && m_currentOpMode && !m_opmodePeriodic) {
    StartCurrentOpMode();
  }

  // Call periodic functions based on current state
  HAL_ObserveUserProgram(word.GetValue());
  if (!enabled) {
    DisabledPeriodic();
    m_watchdog.AddEpoch("DisabledPeriodic()");

    // Call opmode DisabledPeriodic if we have one
    if (m_currentOpMode) {
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

  wpi::tunables::TunableRegistry::Update();
  m_watchdog.AddEpoch("TunableRegistry::Update()");

  if constexpr (IsSimulation()) {
    HAL_SimPeriodicBefore();
    SimulationPeriodic();
    HAL_SimPeriodicAfter();
    m_watchdog.AddEpoch("SimulationPeriodic()");
  }

  m_watchdog.Disable();

  // Flush NetworkTables
  wpi::nt::NetworkTableInstance::GetDefault().FlushLocal();

  // Warn on loop time overruns, and clear the alert once the loop is back on
  // time
  if (m_watchdog.IsExpired()) {
    m_watchdog.PrintEpochs();
  }
  m_loopOverrunAlert.Set(m_watchdog.IsExpired());
}

void OpModeRobotBase::StartCompetition() {
  if constexpr (IsSimulation()) {
    SimulationInit();
  }

  // Tell the DS that the robot is ready to be enabled
  std::puts("\n********** Robot program startup complete **********");
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

void OpModeRobotBase::AddOpModeFactory(RobotMode mode, std::string_view name,
                                       std::string_view group,
                                       std::string_view description,
                                       const wpi::util::Color& textColor,
                                       const wpi::util::Color& backgroundColor,
                                       OpModeFactory factory) {
  int64_t id = RobotState::AddOpMode(mode, name, group, description, textColor,
                                     backgroundColor);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::AddOpModeFactory(RobotMode mode, std::string_view name,
                                       std::string_view group,
                                       std::string_view description,
                                       OpModeFactory factory) {
  int64_t id = RobotState::AddOpMode(mode, name, group, description);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::AddOpModeFactory(RobotMode mode, std::string_view name,
                                       OpModeFactory factory) {
  AddOpModeFactory(mode, name, {}, {}, std::move(factory));
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

  std::print("********** Starting OpMode {} **********\n", m_currentOpModeName);

  // Register the main opmode periodic callback. Capture a weak_ptr so a queued
  // callback can never resurrect or outlive a destroyed opmode.
  m_opmodePeriodic = wpi::internal::PeriodicPriorityQueue::Callback{
      [this, op = std::weak_ptr<OpMode>{m_currentOpMode}] {
        if (auto shared_op = op.lock()) {
          m_opModeWatchdog.Reset();
          shared_op->Periodic();
          m_opModeWatchdog.Disable();

          // Alert on opmode Periodic() overruns, and clear the alert once it's
          // back on time
          m_opModePeriodicOverrunAlert.Set(m_opModeWatchdog.IsExpired());
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
    std::print("********** Ending OpMode {} **********\n", m_currentOpModeName);

    m_currentOpMode->End();
    m_watchdog.AddEpoch("OpMode::End()");

    m_callbacks.Remove(*m_opmodePeriodic);
    m_opmodePeriodic.reset();
    m_opModePeriodicOverrunAlert.Set(false);
  }

  // The additional GetCallbacks() callbacks are registered immediately on
  // construction (even while disabled), so always remove them regardless of
  // whether the opmode was started.
  for (auto& cb : m_activeOpModeCallbacks) {
    m_callbacks.Remove(cb);
  }
  m_activeOpModeCallbacks.clear();

  // Regardless of whether opmode was started, destroy it
  std::print("********** Closing OpMode {} **********\n", m_currentOpModeName);
  m_currentOpMode.reset();
}
