// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/sysid/SysIdRoutineLog.h"

#include <string>

#include <fmt/format.h>

#include "frc/DataLogManager.h"

using namespace frc::sysid;

SysIdRoutineLog::SysIdRoutineLog(std::string_view logName)
    : m_logName(logName) {}

SysIdRoutineLog::MotorLog::MotorLog(std::string_view motorName,
                                    std::string_view logName,
                                    LogEntries* logEntries)
    : m_motorName(motorName), m_logName(logName), m_logEntries(logEntries) {
  (*logEntries)[motorName] = MotorEntries();
}

SysIdRoutineLog::MotorLog& SysIdRoutineLog::MotorLog::value(
    std::string_view name, double value, std::string_view unit) {
  auto& motorEntries = (*m_logEntries)[m_motorName];

  if (!motorEntries.contains(name)) {
    wpi::log::DataLog& log = frc::DataLogManager::GetLog();

    motorEntries[name] = wpi::log::DoubleLogEntry(
        log, fmt::format("{}-{}-{}", name, m_motorName, m_logName), unit);
  }

  motorEntries[name].Append(value);
  return *this;
}

SysIdRoutineLog::MotorLog SysIdRoutineLog::Motor(std::string_view motorName) {
  return MotorLog{motorName, m_logName, &m_logEntries};
}

void SysIdRoutineLog::RecordState(State state) {
  if (!m_stateInitialized) {
    m_state =
        wpi::log::StringLogEntry{frc::DataLogManager::GetLog(),
                                 fmt::format("sysid-test-state-{}", m_logName)};
    m_stateInitialized = true;
  }
  m_state.Append(StateEnumToString(state));
}

std::string SysIdRoutineLog::StateEnumToString(State state) {
  switch (state) {
    case State::kQuasistaticForward:
      return "quasistatic-forward";
    case State::kQuasistaticReverse:
      return "quasistatic-reverse";
    case State::kDynamicForward:
      return "dynamic-forward";
    case State::kDynamicReverse:
      return "dynamic-reverse";
    case State::kNone:
      return "none";
    default:
      return "none";
  }
}
