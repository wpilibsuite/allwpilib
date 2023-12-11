// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.sysid;

import static edu.wpi.first.units.Units.Seconds;
import static edu.wpi.first.units.Units.Volts;
import static java.util.Map.entry;

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
public class SysIdRoutineLog implements MotorLog {
  private final Map<String, Map<String, DoubleLogEntry>> m_logEntries = new HashMap<>();
  private final String m_logName;
  private final StringLogEntry m_state;

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
    m_state = new StringLogEntry(DataLogManager.getLog(), "sysid-test-state-" + logName);
    m_state.append(State.kNone.toString());
  }

  /** Possible state of a SysId routine. */
  public enum State {
    kQuasistaticForward("quasistatic-forward"),
    kQuasistaticReverse("quasistatic-reverse"),
    kDynamicForward("dynamic-forward"),
    kDynamicReverse("dynamic-reverse"),
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

  @Override
  public void recordFrame(
      double voltage, double distance, double velocity, String motorName, String distanceUnit) {
    var motorEntries = m_logEntries.get(motorName);

    if (motorEntries == null) {
      var log = DataLogManager.getLog();

      motorEntries =
          Map.ofEntries(
              entry(
                  "voltage",
                  new DoubleLogEntry(log, "voltage-" + motorName + "-" + m_logName, Volts.name())),
              entry(
                  "distance",
                  new DoubleLogEntry(log, "distance-" + motorName + "-" + m_logName, distanceUnit)),
              entry(
                  "velocity",
                  new DoubleLogEntry(
                      log,
                      "velocity-" + motorName + "-" + m_logName,
                      distanceUnit + "-per-" + Seconds.name())));

      m_logEntries.put(motorName, motorEntries);
    }

    motorEntries.get("voltage").append(voltage);
    motorEntries.get("distance").append(distance);
    motorEntries.get("velocity").append(velocity);
  }

  /**
   * Records the current state of the SysId test routine. Should be called once per iteration during
   * tests with the type of the current test, and once upon test end with state `none`.
   *
   * @param state The current state of the SysId test routine.
   */
  public void recordState(State state) {
    m_state.append(state.toString());
  }
}
