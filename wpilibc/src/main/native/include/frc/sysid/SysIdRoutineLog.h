// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

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

/**
 * Possible state of a SysId routine.
 */
enum class State {
  /// Quasistatic forward test.
  kQuasistaticForward,
  /// Quasistatic reverse test.
  kQuasistaticReverse,
  /// Dynamic forward test.
  kDynamicForward,
  /// Dynamic reverse test.
  kDynamicReverse,
  /// No test.
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
  /** Logs data from a single motor during a SysIdRoutine. */
  class MotorLog {
   public:
    /**
     * Log a generic data value from the motor.
     *
     * @param name The name of the data field being recorded.
     * @param value The numeric value of the data field.
     * @param unit The unit string of the data field.
     * @return The motor log (for call chaining).
     */
    MotorLog& value(std::string_view name, double value, std::string_view unit);

    /**
     * Log the voltage applied to the motor.
     *
     * @param voltage The voltage to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& voltage(units::volt_t voltage) {
      return value("voltage", voltage.value(), voltage.name());
    }

    /**
     * Log the linear position of the motor.
     *
     * @param position The linear position to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& position(units::meter_t position) {
      return value("position", position.value(), position.name());
    }

    /**
     * Log the angular position of the motor.
     *
     * @param position The angular position to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& position(units::turn_t position) {
      return value("position", position.value(), position.name());
    }

    /**
     * Log the linear velocity of the motor.
     *
     * @param velocity The linear velocity to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& velocity(units::meters_per_second_t velocity) {
      return value("velocity", velocity.value(), velocity.name());
    }

    /**
     * Log the angular velocity of the motor.
     *
     * @param velocity The angular velocity to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& velocity(units::turns_per_second_t velocity) {
      return value("velocity", velocity.value(), velocity.name());
    }

    /**
     * Log the linear acceleration of the motor.
     *
     * This is optional; SysId can perform an accurate fit without it.
     *
     * @param acceleration The linear acceleration to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& acceleration(units::meters_per_second_squared_t acceleration) {
      return value("acceleration", acceleration.value(), acceleration.name());
    }

    /**
     * Log the angular acceleration of the motor.
     *
     * This is optional; SysId can perform an accurate fit without it.
     *
     * @param acceleration The angular acceleration to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& acceleration(units::turns_per_second_squared_t acceleration) {
      return value("acceleration", acceleration.value(), acceleration.name());
    }

    /**
     * Log the current applied to the motor.
     *
     * This is optional; SysId can perform an accurate fit without it.
     *
     * @param current The current to record.
     * @return The motor log (for call chaining).
     */
    MotorLog& current(units::ampere_t current) {
      return value("current", current.value(), current.name());
    }

   private:
    friend class SysIdRoutineLog;
    /**
     * Create a new SysId motor log handle.
     *
     * @param motorName The name of the motor whose data is being logged.
     * @param logName The name of the SysIdRoutineLog that this motor belongs
     * to.
     * @param logEntries The DataLog entries of the SysIdRoutineLog that this
     * motor belongs to.
     */
    MotorLog(std::string_view motorName, std::string_view logName,
             LogEntries* logEntries);
    std::string m_motorName;
    std::string m_logName;
    LogEntries* m_logEntries;
  };

  /**
   * Create a new logging utility for a SysId test routine.
   *
   * @param logName The name for the test routine in the log. Should be unique
   *   between complete test routines (quasistatic and dynamic, forward and
   *   reverse). The current state of this test (e.g. "quasistatic-forward")
   *   will appear in WPILog under the "sysid-test-state-logName" entry.
   */
  explicit SysIdRoutineLog(std::string_view logName);

  /**
   * Records the current state of the SysId test routine. Should be called once
   * per iteration during tests with the type of the current test, and once upon
   * test end with state `none`.
   *
   * @param state The current state of the SysId test routine.
   */
  void RecordState(State state);

  /**
   * Log data from a motor during a SysId routine.
   *
   * @param motorName The name of the motor.
   * @return Handle with chainable callbacks to log individual data fields.
   */
  MotorLog Motor(std::string_view motorName);

  static std::string StateEnumToString(State state);

 private:
  LogEntries m_logEntries;
  std::string m_logName;
  bool m_stateInitialized = false;
  wpi::log::StringLogEntry m_state;
};
}  // namespace frc::sysid
