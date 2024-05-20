// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/** Log array of double values. */
public class DoubleArrayLogEntry extends DataLogEntry {
  /** The data type for double array values. */
  public static final String kDataType = "double[]";

  /**
   * Constructs a double array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   */
  public DoubleArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, kDataType, metadata, timestamp);
  }

  /**
   * Constructs a double array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   */
  public DoubleArrayLogEntry(DataLog log, String name, String metadata) {
    this(log, name, metadata, 0);
  }

  /**
   * Constructs a double array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param timestamp entry creation timestamp (0=now)
   */
  public DoubleArrayLogEntry(DataLog log, String name, long timestamp) {
    this(log, name, "", timestamp);
  }

  /**
   * Constructs a double array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   */
  public DoubleArrayLogEntry(DataLog log, String name) {
    this(log, name, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(double[] value, long timestamp) {
    m_log.appendDoubleArray(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(double[] value) {
    m_log.appendDoubleArray(m_entry, value, 0);
  }
}
