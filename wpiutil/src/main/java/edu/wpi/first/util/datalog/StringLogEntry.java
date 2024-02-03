// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/** Log string values. */
public class StringLogEntry extends DataLogEntry {
  /** The data type for string values. */
  public static final String kDataType = "string";

  /**
   * Constructs a String log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param type Data type
   * @param timestamp entry creation timestamp (0=now)
   */
  public StringLogEntry(DataLog log, String name, String metadata, String type, long timestamp) {
    super(log, name, type, metadata, timestamp);
  }

  /**
   * Constructs a String log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param type Data type
   */
  public StringLogEntry(DataLog log, String name, String metadata, String type) {
    this(log, name, metadata, type, 0);
  }

  /**
   * Constructs a String log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   */
  public StringLogEntry(DataLog log, String name, String metadata, long timestamp) {
    this(log, name, metadata, kDataType, timestamp);
  }

  /**
   * Constructs a String log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   */
  public StringLogEntry(DataLog log, String name, String metadata) {
    this(log, name, metadata, 0);
  }

  /**
   * Constructs a String log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param timestamp entry creation timestamp (0=now)
   */
  public StringLogEntry(DataLog log, String name, long timestamp) {
    this(log, name, "", timestamp);
  }

  /**
   * Constructs a String log entry.
   *
   * @param log datalog
   * @param name name of the entry
   */
  public StringLogEntry(DataLog log, String name) {
    this(log, name, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(String value, long timestamp) {
    m_log.appendString(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(String value) {
    m_log.appendString(m_entry, value, 0);
  }
}
