// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/** Log boolean values. */
public class BooleanLogEntry extends DataLogEntry {
  public static final String kDataType = "boolean";

  public BooleanLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, kDataType, metadata, timestamp);
  }

  public BooleanLogEntry(DataLog log, String name, String metadata) {
    this(log, name, metadata, 0);
  }

  public BooleanLogEntry(DataLog log, String name, long timestamp) {
    this(log, name, "", timestamp);
  }

  public BooleanLogEntry(DataLog log, String name) {
    this(log, name, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  public void append(boolean value, long timestamp) {
    m_log.appendBoolean(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(boolean value) {
    m_log.appendBoolean(m_entry, value, 0);
  }
}
