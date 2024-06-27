// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.sysid;

import static edu.wpi.first.units.Units.Amps;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.Rotations;
import static edu.wpi.first.units.Units.RotationsPerSecond;
import static edu.wpi.first.units.Units.Second;
import static edu.wpi.first.units.Units.Volts;

import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.AngularAcceleration;
import edu.wpi.first.units.measure.AngularVelocity;
import edu.wpi.first.units.measure.Current;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearAcceleration;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Voltage;
import edu.wpi.first.util.datalog.DoubleLogEntry;
import edu.wpi.first.util.datalog.StringLogEntry;
import edu.wpi.first.wpilibj.DataLogManager;
import java.util.HashMap;
import java.util.Map;

/**
 * Utility for logging data from a SysId test routine. Each complete routine (quasistatic and
 * dynamic, forward and reverse) should have its own SysIdRoutineLog instance, with a unique log
 * name.
 */
public class SysIdRoutineLog {
  private final Map<String, Map<String, DoubleLogEntry>> m_logEntries = new HashMap<>();
  private final String m_logName;
  private StringLogEntry m_state;

  /**
   * Create a new logging utility for a SysId test routine.
   *
   * @param logName The name for the test routine in the log. Should be unique between complete test
   *     routines (quasistatic and dynamic, forward and reverse). The current state of this test
   *     (e.g. "quasistatic-forward") will appear in WPILog under the "sysid-test-state-logName"
   *     entry.
   */
  public SysIdRoutineLog(String logName) {
    m_logName = logName;
  }

  /** Possible state of a SysId routine. */
  public enum State {
    /** Quasistatic forward test. */
    kQuasistaticForward("quasistatic-forward"),
    /** Quasistatic reverse test. */
    kQuasistaticReverse("quasistatic-reverse"),
    /** Dynamic forward test. */
    kDynamicForward("dynamic-forward"),
    /** Dynamic reverse test. */
    kDynamicReverse("dynamic-reverse"),
    /** No test. */
    kNone("none");

    private final String m_state;

    State(String state) {
      m_state = state;
    }

    @Override
    public String toString() {
      return m_state;
    }
  }

  /** Logs data from a single motor during a SysIdRoutine. */
  public final class MotorLog {
    private final String m_motorName;

    /**
     * Create a new SysId motor log handle.
     *
     * @param motorName The name of the motor whose data is being logged.
     */
    private MotorLog(String motorName) {
      m_motorName = motorName;
      m_logEntries.put(motorName, new HashMap<>());
    }

    /**
     * Log a generic data value from the motor.
     *
     * @param name The name of the data field being recorded.
     * @param value The numeric value of the data field.
     * @param unit The unit string of the data field.
     * @return The motor log (for call chaining).
     */
    public MotorLog value(String name, double value, String unit) {
      var motorEntries = m_logEntries.get(m_motorName);
      var entry = motorEntries.get(name);

      if (entry == null) {
        var log = DataLogManager.getLog();

        entry = new DoubleLogEntry(log, name + "-" + m_motorName + "-" + m_logName, unit);
        motorEntries.put(name, entry);
      }

      entry.append(value);
      return this;
    }

    /**
     * Log the voltage applied to the motor.
     *
     * @param voltage The voltage to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog voltage(Voltage voltage) {
      return value("voltage", voltage.in(Volts), Volts.name());
    }

    /**
     * Log the linear position of the motor.
     *
     * @param position The linear position to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog linearPosition(Distance position) {
      return value("position", position.in(Meters), Meters.name());
    }

    /**
     * Log the angular position of the motor.
     *
     * @param position The angular position to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog angularPosition(Angle position) {
      return value("position", position.in(Rotations), Rotations.name());
    }

    /**
     * Log the linear velocity of the motor.
     *
     * @param velocity The linear velocity to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog linearVelocity(LinearVelocity velocity) {
      return value("velocity", velocity.in(MetersPerSecond), MetersPerSecond.name());
    }

    /**
     * Log the angular velocity of the motor.
     *
     * @param velocity The angular velocity to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog angularVelocity(AngularVelocity velocity) {
      return value("velocity", velocity.in(RotationsPerSecond), RotationsPerSecond.name());
    }

    /**
     * Log the linear acceleration of the motor.
     *
     * <p>This is optional; SysId can perform an accurate fit without it.
     *
     * @param acceleration The linear acceleration to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog linearAcceleration(LinearAcceleration acceleration) {
      return value(
          "acceleration",
          acceleration.in(MetersPerSecond.per(Second)),
          MetersPerSecond.per(Second).name());
    }

    /**
     * Log the angular acceleration of the motor.
     *
     * <p>This is optional; SysId can perform an accurate fit without it.
     *
     * @param acceleration The angular acceleration to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog angularAcceleration(AngularAcceleration acceleration) {
      return value(
          "acceleration",
          acceleration.in(RotationsPerSecond.per(Second)),
          RotationsPerSecond.per(Second).name());
    }

    /**
     * Log the current applied to the motor.
     *
     * <p>This is optional; SysId can perform an accurate fit without it.
     *
     * @param current The current to record.
     * @return The motor log (for call chaining).
     */
    public MotorLog current(Current current) {
      value("current", current.in(Amps), Amps.name());
      return this;
    }
  }

  /**
   * Log data from a motor during a SysId routine.
   *
   * @param motorName The name of the motor.
   * @return Handle with chainable callbacks to log individual data fields.
   */
  public MotorLog motor(String motorName) {
    return new MotorLog(motorName);
  }

  /**
   * Records the current state of the SysId test routine. Should be called once per iteration during
   * tests with the type of the current test, and once upon test end with state `none`.
   *
   * @param state The current state of the SysId test routine.
   */
  public void recordState(State state) {
    if (m_state == null) {
      m_state = new StringLogEntry(DataLogManager.getLog(), "sysid-test-state-" + m_logName);
    }
    m_state.append(state.toString());
  }
}
