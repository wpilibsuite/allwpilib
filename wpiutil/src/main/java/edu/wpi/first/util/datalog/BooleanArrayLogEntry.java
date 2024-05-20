// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/** Log array of boolean values. */
public class BooleanArrayLogEntry extends DataLogEntry {
  /** The data type for boolean array values. */
  public static final String kDataType = "boolean[]";

  /**
   * Constructs a boolean array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   */
  public BooleanArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, kDataType, metadata, timestamp);
  }

  /**
   * Constructs a boolean array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   */
  public BooleanArrayLogEntry(DataLog log, String name, String metadata) {
    this(log, name, metadata, 0);
  }

  /**
   * Constructs a boolean array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param timestamp entry creation timestamp (0=now)
   */
  public BooleanArrayLogEntry(DataLog log, String name, long timestamp) {
    this(log, name, "", timestamp);
  }

  /**
   * Constructs a boolean array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   */
  public BooleanArrayLogEntry(DataLog log, String name) {
    this(log, name, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(boolean[] value, long timestamp) {
    m_log.appendBooleanArray(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(boolean[] value) {
    m_log.appendBooleanArray(m_entry, value, 0);
  }
}
