// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <unordered_map>

#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_acceleration.h>
#include <units/angular_velocity.h>
#include <units/current.h>
#include <units/length.h>
#include <units/velocity.h>
#include <units/voltage.h>
#include <wpi/DataLog.h>

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
class SysIdRoutineLog {
  using MotorEntries = wpi::StringMap<wpi::log::DoubleLogEntry>;
  using LogEntries = wpi::StringMap<MotorEntries>;

 public:
  class MotorLog {
   public:
    MotorLog(const std::string& motorName, const std::string& logName,
             LogEntries* logEntries);

    MotorLog& value(const std::string& name, double value,
                    const std::string& unit);

    MotorLog& voltage(units::volt_t voltage) {
      return value("voltage", voltage.value(), voltage.name());
    }

    MotorLog& position(units::meter_t position) {
      return value("position", position.value(), position.name());
    }

    MotorLog& position(units::turn_t position) {
      return value("position", position.value(), position.name());
    }

    MotorLog& velocity(units::meters_per_second_t velocity) {
      return value("velocity", velocity.value(), velocity.name());
    }

    MotorLog& velocity(units::turns_per_second_t velocity) {
      return value("velocity", velocity.value(), velocity.name());
    }

    MotorLog& acceleration(units::meters_per_second_squared_t acceleration) {
      return value("acceleration", acceleration.value(), acceleration.name());
    }

    MotorLog& acceleration(units::turns_per_second_squared_t acceleration) {
      return value("acceleration", acceleration.value(), acceleration.name());
    }

    MotorLog& current(units::ampere_t current) {
      return value("current", current.value(), current.name());
    }

   private:
    std::string m_motorName;
    std::string m_logName;
    LogEntries* m_logEntries;
  };
  /**
   * Create a new logging utility for a SysId test routine.
   *
   * @param logName The name for the test routine in the log. Should be unique
   * between complete test routines (quasistatic and dynamic, forward and
   * reverse). The current state of this test (e.g. "quasistatic-forward") will
   * appear in WPILog under the "sysid-test-state-logName" entry.
   */
  explicit SysIdRoutineLog(const std::string& logName);
  /**
   * Records the current state of the SysId test routine. Should be called once
   * per iteration during tests with the type of the current test, and once upon
   * test end with state `none`.
   *
   * @param state The current state of the SysId test routine.
   */
  void RecordState(State state);

  MotorLog Motor(const std::string& motorName);

  static std::string StateEnumToString(State state);

 private:
  LogEntries m_logEntries{};
  std::string m_logName{};
  wpi::log::StringLogEntry m_state{};
};
}  // namespace frc::sysid
