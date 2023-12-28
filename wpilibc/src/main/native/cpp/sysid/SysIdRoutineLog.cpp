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

void SysIdRoutineLog::RecordFrameLinear(units::volt_t voltage,
                                        units::meter_t distance,
                                        units::meters_per_second_t velocity,
                                        const std::string& motorName) {
  if (!m_logEntries.contains(motorName)) {
    wpi::log::DataLog& log = frc::DataLogManager::GetLog();

    m_logEntries[motorName].emplace(
        "voltage", wpi::log::DoubleLogEntry(
                       log, "voltage-" + motorName + "-" + m_logName, "Volt"));
    m_logEntries[motorName].emplace(
        "distance",
        wpi::log::DoubleLogEntry(log, "distance-" + motorName + "-" + m_logName,
                                 "meters"));
    m_logEntries[motorName].emplace(
        "velocity",
        wpi::log::DoubleLogEntry(log, "velocity-" + motorName + "-" + m_logName,
                                 "meters-per-second"));
  }

  m_logEntries[motorName]["voltage"].Append(voltage.value());
  m_logEntries[motorName]["distance"].Append(distance.value());
  m_logEntries[motorName]["velocity"].Append(velocity.value());
}

void SysIdRoutineLog::RecordFrameAngular(units::volt_t voltage,
                                         units::turn_t angle,
                                         units::turns_per_second_t velocity,
                                         const std::string& motorName) {
  if (!m_logEntries.contains(motorName)) {
    wpi::log::DataLog& log = frc::DataLogManager::GetLog();

    m_logEntries[motorName].emplace(
        "voltage", wpi::log::DoubleLogEntry(
                       log, "voltage-" + motorName + "-" + m_logName, "Volt"));
    m_logEntries[motorName].emplace(
        "distance",
        wpi::log::DoubleLogEntry(log, "distance-" + motorName + "-" + m_logName,
                                 "rotations"));
    m_logEntries[motorName].emplace(
        "velocity",
        wpi::log::DoubleLogEntry(log, "velocity-" + motorName + "-" + m_logName,
                                 "rotations-per-second"));
  }

  m_logEntries[motorName]["voltage"].Append(voltage.value());
  m_logEntries[motorName]["distance"].Append(angle.value());
  m_logEntries[motorName]["velocity"].Append(velocity.value());
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
