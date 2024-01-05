// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/sysid/SysIdRoutineLog.h"

#include "frc/DataLogManager.h"

using namespace frc::sysid;

SysIdRoutineLog::SysIdRoutineLog(const std::string& logName)
    : m_logName(logName),
      m_state(wpi::log::StringLogEntry{frc::DataLogManager::GetLog(),
                                       "sysid-test-state" + logName}) {
  m_state.Append(StateEnumToString(State::kNone));
}

SysIdRoutineLog::MotorLog::MotorLog(const std::string& motorName, const std::string& logName, LogEntries* logEntries)
    : m_motorName(motorName), m_logName(logName), m_logEntries(logEntries) {
  (*logEntries)[motorName] = MotorEntries();
}

SysIdRoutineLog::MotorLog& SysIdRoutineLog::MotorLog::value(const std::string& name, double value, const std::string& unit) {
  auto& motorEntries = (*m_logEntries)[m_motorName];

  if (!motorEntries.contains(name)) {
    wpi::log::DataLog& log = frc::DataLogManager::GetLog();

    motorEntries[name] = wpi::log::DoubleLogEntry(log, name + "-" + m_motorName + "-" + m_logName, unit);
  }

  motorEntries[name].Append(value);
  return *this;
}

SysIdRoutineLog::MotorLog SysIdRoutineLog::Motor(const std::string& motorName) {
  return MotorLog{motorName, m_logName, &m_logEntries};
}

void SysIdRoutineLog::RecordState(State state) {
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
