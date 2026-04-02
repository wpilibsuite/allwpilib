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

#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/Notifier.hpp"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/opmode/OpMode.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/RobotController.hpp"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/Synchronization.h"

using namespace wpi;

OpModeRobotBase::OpModeRobotBase(wpi::units::second_t period)
    : m_period{period} {
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
  DriverStation::RefreshData();

  // Get current enabled state and opmode
  hal::ControlWord word = DriverStation::GetControlWord();
  bool enabled = word.IsEnabled();
  int64_t modeId = word.IsDSAttached() ? word.GetOpModeId() : 0;

  if (!m_calledDriverStationConnected && word.IsDSAttached()) {
    m_calledDriverStationConnected = true;
    DriverStationConnected();
  }

  // Handle opmode changes
  if (modeId != m_lastModeId) {
    // Clean up current opmode
    if (m_currentOpMode) {
      // Remove opmode callbacks
      if (m_opmodePeriodic) {
        m_callbacks.Remove(*m_opmodePeriodic);
        m_opmodePeriodic.reset();
      }
      for (auto& cb : m_activeOpModeCallbacks) {
        m_callbacks.Remove(cb);
      }
      m_activeOpModeCallbacks.clear();
      m_currentOpMode.reset();
    }

    // Set up new opmode
    if (modeId != 0) {
      auto data = m_opModes.lookup(modeId);
      if (data.factory) {
        // Instantiate the new opmode
        fmt::print("********** Starting OpMode {} **********\n", data.name);
        m_currentOpMode = data.factory();
        if (m_currentOpMode) {
          // Ensure disabledPeriodic is called at least once
          m_currentOpMode->DisabledPeriodic();
          // Register the opmode's periodic callbacks
          m_opmodePeriodic = wpi::internal::PeriodicPriorityQueue::Callback{
              [op = m_currentOpMode.get()] { op->Periodic(); }, m_startTime,
              m_period};
          m_callbacks.Add(*m_opmodePeriodic);
          m_activeOpModeCallbacks = m_currentOpMode->GetCallbacks();
          for (auto& cb : m_activeOpModeCallbacks) {
            m_callbacks.Add(cb);
          }
        }
      } else {
        WPILIB_ReportError(err::Error, "No OpMode found for mode {}", modeId);
      }
    }
    m_lastModeId = modeId;
  }

  // Handle enabled state changes
  if (m_lastEnabledState != enabled) {
    if (enabled) {
      // Transitioning to enabled
      DisabledExit();
      if (m_currentOpMode) {
        m_currentOpMode->Start();
      }
    } else {
      // Transitioning to disabled
      if (m_currentOpMode && m_lastEnabledState) {
        // Was enabled, now disabled
        m_currentOpMode->End();
      }
      DisabledInit();
    }
    m_lastEnabledState = enabled;
  }

  // Call periodic functions based on current state
  if (!enabled) {
    // Only call DisabledPeriodic if we didn't just call DisabledInit
    static bool justCalledDisabledInit = false;
    if (m_lastEnabledState != enabled) {
      justCalledDisabledInit = true;
    } else {
      if (!justCalledDisabledInit) {
        DisabledPeriodic();
      }
      justCalledDisabledInit = false;
    }

    // Call opmode DisabledPeriodic if we have one
    if (m_currentOpMode) {
      m_currentOpMode->DisabledPeriodic();
    }
  }

  // Call NonePeriodic when no opmode is selected
  if (modeId == 0) {
    NonePeriodic();
  }

  // Always call RobotPeriodic
  RobotPeriodic();

  // Always observe user program state
  HAL_ObserveUserProgram(word.GetValue());

  // Call SimulationPeriodic if in simulation
  if constexpr (IsSimulation()) {
    SimulationPeriodic();
  }
}

void OpModeRobotBase::StartCompetition() {
  fmt::print("********** Robot program startup complete **********\n");

  if constexpr (IsSimulation()) {
    SimulationInit();
  }

  // Tell the DS that the robot is ready to be enabled
  HAL_ObserveUserProgramStarting();

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
  int64_t id = DriverStation::AddOpMode(mode, name, group, description,
                                        textColor, backgroundColor);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::AddOpModeFactory(OpModeFactory factory, RobotMode mode,
                                       std::string_view name,
                                       std::string_view group,
                                       std::string_view description) {
  int64_t id = DriverStation::AddOpMode(mode, name, group, description);
  if (id != 0) {
    m_opModes[id] = OpModeData{std::string{name}, std::move(factory)};
  }
}

void OpModeRobotBase::RemoveOpMode(RobotMode mode, std::string_view name) {
  int64_t id = DriverStation::RemoveOpMode(mode, name);
  if (id != 0) {
    m_opModes.erase(id);
  }
}

void OpModeRobotBase::PublishOpModes() {
  DriverStation::PublishOpModes();
}

void OpModeRobotBase::ClearOpModes() {
  DriverStation::ClearOpModes();
  m_opModes.clear();
}
