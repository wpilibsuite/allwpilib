// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <unordered_map>

#include <wpi/DataLog.h>

#include "frc/sysid/MotorLog.h"

namespace frc::sysid {

/** Possible state of a SysId routine. */
enum class State {
  kQuasistaticForward,
  kQuasistaticReverse,
  kDynamicForward,
  kDynamicReverse,
  kNone
};

/**
 * Utility for logging data from a SysId test routine. Each complete routine
 * (quasistatic and dynamic, forward and reverse) should have its own
 * SysIdRoutineLog instance, with a unique log name.
 */
class SysIdRoutineLog : public MotorLog {
 public:
  /**
   * Create a new logging utility for a SysId test routine.
   *
   * @param logName The name for the test routine in the log. Should be unique
   * between complete test routines (quasistatic and dynamic, forward and
   * reverse). The current state of this test (e.g. "quasistatic-forward") will
   * appear in WPILog under the "sysid-test-state-logName" entry.
   */
  explicit SysIdRoutineLog(const std::string& logName);
  void RecordFrameLinear(units::volt_t voltage, units::meter_t distance,
                         units::meters_per_second_t velocity,
                         const std::string& motorName) override;
  void RecordFrameAngular(units::volt_t voltage, units::turn_t angle,
                          units::turns_per_second_t velocity,
                          const std::string& motorName) override;
  /**
   * Records the current state of the SysId test routine. Should be called once
   * per iteration during tests with the type of the current test, and once upon
   * test end with state `none`.
   *
   * @param state The current state of the SysId test routine.
   */
  void RecordState(State state);
  static std::string StateEnumToString(State state);

 private:
  std::unordered_map<std::string,
                     std::unordered_map<std::string, wpi::log::DoubleLogEntry>>
      m_logEntries{};
  std::string m_logName{};
  wpi::log::StringLogEntry m_state{};
};
}  // namespace frc::sysid
