// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <frc/Timer.h>
#include <frc/sysid/SysIdRoutineLog.h>

#include "frc2/command/CommandPtr.h"
#include "frc2/command/Subsystem.h"

namespace frc2::sysid {

using ramp_rate_t = units::unit_t<
    units::compound_unit<units::volt, units::inverse<units::second>>>;

/** Hardware-independent configuration for a SysId test routine. */
class Config {
 public:
  /// The voltage ramp rate used for quasistatic test routines.
  ramp_rate_t m_rampRate{1_V / 1_s};

  /// The step voltage output used for dynamic test routines.
  units::volt_t m_stepVoltage{7_V};

  /// Safety timeout for the test routine commands.
  units::second_t m_timeout{10_s};

  /// Optional handle for recording test state in a third-party logging
  /// solution.
  std::function<void(frc::sysid::State)> m_recordState;

  /**
   * Create a new configuration for a SysId test routine.
   *
   * @param rampRate The voltage ramp rate used for quasistatic test routines.
   *   Defaults to 1 volt per second if left null.
   * @param stepVoltage The step voltage output used for dynamic test routines.
   *   Defaults to 7 volts if left null.
   * @param timeout Safety timeout for the test routine commands. Defaults to 10
   *   seconds if left null.
   * @param recordState Optional handle for recording test state in a
   *   third-party logging solution. If provided, the test routine state will be
   *   passed to this callback instead of logged in WPILog.
   */
  Config(std::optional<ramp_rate_t> rampRate,
         std::optional<units::volt_t> stepVoltage,
         std::optional<units::second_t> timeout,
         std::function<void(frc::sysid::State)> recordState)
      : m_recordState{recordState} {
    if (rampRate) {
      m_rampRate = rampRate.value();
    }
    if (stepVoltage) {
      m_stepVoltage = stepVoltage.value();
    }
    if (timeout) {
      m_timeout = timeout.value();
    }
  }
};

class Mechanism {
 public:
  /// Sends the SysId-specified drive signal to the mechanism motors during test
  /// routines.
  std::function<void(units::volt_t)> m_drive;

  /// Returns measured data (voltages, positions, velocities) of the mechanism
  /// motors during test routines.
  std::function<void(frc::sysid::SysIdRoutineLog*)> m_log;

  /// The subsystem containing the motor(s) that is (or are) being
  /// characterized.
  frc2::Subsystem* m_subsystem;

  /// The name of the mechanism being tested. Will be appended to the log entry
  /// title for the routine's test state, e.g. "sysid-test-state-mechanism".
  std::string m_name;

  /**
   * Create a new mechanism specification for a SysId routine.
   *
   * @param drive Sends the SysId-specified drive signal to the mechanism motors
   * during test routines.
   * @param log Returns measured data of the mechanism motors during test
   * routines. To return data, call `Motor(string motorName)` on the supplied
   * `SysIdRoutineLog` instance, and then call one or more of the chainable
   * logging handles (e.g. `voltage`) on the returned `MotorLog`.  Multiple
   * motors can be logged in a single callback by calling `Motor` multiple
   * times.
   * @param subsystem The subsystem containing the motor(s) that is (or are)
   *   being characterized. Will be declared as a requirement for the returned
   *   test commands.
   * @param name The name of the mechanism being tested. Will be appended to the
   *   log entry * title for the routine's test state, e.g.
   *   "sysid-test-state-mechanism". Defaults to the name of the subsystem if
   *   left null.
   */
  Mechanism(std::function<void(units::volt_t)> drive,
            std::function<void(frc::sysid::SysIdRoutineLog*)> log,
            frc2::Subsystem* subsystem, std::string_view name)
      : m_drive{std::move(drive)},
        m_log{log ? std::move(log) : [](frc::sysid::SysIdRoutineLog* log) {}},
        m_subsystem{subsystem},
        m_name{name} {}

  /**
   * Create a new mechanism specification for a SysId routine. Defaults the
   * mechanism name to the subsystem name.
   *
   * @param drive Sends the SysId-specified drive signal to the mechanism motors
   * during test routines.
   * @param log Returns measured data of the mechanism motors during test
   * routines. To return data, call `Motor(string motorName)` on the supplied
   * `SysIdRoutineLog` instance, and then call one or more of the chainable
   * logging handles (e.g. `voltage`) on the returned `MotorLog`.  Multiple
   * motors can be logged in a single callback by calling `Motor` multiple
   * times.
   * @param subsystem The subsystem containing the motor(s) that is (or are)
   *   being characterized. Will be declared as a requirement for the returned
   *   test commands. The subsystem's `name` will be appended to the log entry
   *   title for the routine's test state, e.g. "sysid-test-state-subsystem".
   */
  Mechanism(std::function<void(units::volt_t)> drive,
            std::function<void(frc::sysid::SysIdRoutineLog*)> log,
            frc2::Subsystem* subsystem)
      : m_drive{std::move(drive)},
        m_log{log ? std::move(log) : [](frc::sysid::SysIdRoutineLog* log) {}},
        m_subsystem{subsystem},
        m_name{m_subsystem->GetName()} {}
};

/**
 * Motor direction for a SysId test.
 */
enum Direction {
  /// Forward.
  kForward,
  /// Reverse.
  kReverse
};

/**
 * A SysId characterization routine for a single mechanism. Mechanisms may have
 * multiple motors.
 *
 * A single subsystem may have multiple mechanisms, but mechanisms should not
 * share test routines. Each complete test of a mechanism should have its own
 * SysIdRoutine instance, since the log name of the recorded data is determined
 * by the mechanism name.
 *
 * The test state (e.g. "quasistatic-forward") is logged once per iteration
 * during test execution, and once with state "none" when a test ends. Motor
 * frames are logged every iteration during test execution.
 *
 * Timestamps are not coordinated across data, so motor frames and test state
 * tags may be recorded on different log frames. Because frame alignment is not
 * guaranteed, SysId parses the log by using the test state flag to determine
 * the timestamp range for each section of the test, and then extracts the motor
 * frames within the valid timestamp ranges. If a given test was run multiple
 * times in a single logfile, the user will need to select which of the tests to
 * use for the fit in the analysis tool.
 */
class SysIdRoutine : public frc::sysid::SysIdRoutineLog {
 public:
  /**
   * Create a new SysId characterization routine.
   *
   * @param config Hardware-independent parameters for the SysId routine.
   * @param mechanism Hardware interface for the SysId routine.
   */
  SysIdRoutine(Config config, Mechanism mechanism)
      : SysIdRoutineLog(mechanism.m_name),
        m_config(config),
        m_mechanism(mechanism),
        m_recordState(config.m_recordState ? config.m_recordState
                                           : [this](frc::sysid::State state) {
                                               this->RecordState(state);
                                             }) {}

  frc2::CommandPtr Quasistatic(Direction direction);
  frc2::CommandPtr Dynamic(Direction direction);

 private:
  Config m_config;
  Mechanism m_mechanism;
  units::volt_t m_outputVolts{0};
  std::function<void(frc::sysid::State)> m_recordState;
  frc::Timer timer;
};
}  // namespace frc2::sysid
