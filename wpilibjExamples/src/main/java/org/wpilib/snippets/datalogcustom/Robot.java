// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.datalogcustom;

import org.wpilib.datalog.BooleanLogEntry;
import org.wpilib.datalog.DataLog;
import org.wpilib.datalog.DoubleLogEntry;
import org.wpilib.datalog.StringLogEntry;
import org.wpilib.framework.TimedRobot;
import org.wpilib.system.DataLogManager;

/**
 * Snippets that logs custom data to the WPILib data log for wpilib-docs.
 * https://docs.wpilib.org/en/stable/docs/software/telemetry/datalog.html
 */
public class Robot extends TimedRobot {
  private BooleanLogEntry m_booleanLog;
  private DoubleLogEntry m_doubleLog;
  private StringLogEntry m_stringLog;

  /** Set up the custom entries. */
  public Robot() {
    // Starts recording to data log.
    DataLogManager.start();

    // Set up custom log entries.
    DataLog log = DataLogManager.getLog();
    m_booleanLog = new BooleanLogEntry(log, "/my/boolean");
    m_doubleLog = new DoubleLogEntry(log, "/my/double");
    m_stringLog = new StringLogEntry(log, "/my/string");
  }

  /** Log the custom entries periodically. */
  @Override
  public void teleopPeriodic() {
    m_booleanLog.append(true);
    m_doubleLog.append(3.5);
    m_stringLog.append("example");
  }
}
