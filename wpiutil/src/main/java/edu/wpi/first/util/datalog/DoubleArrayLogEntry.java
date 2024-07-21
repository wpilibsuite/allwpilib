// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.util.Arrays;

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

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public synchronized void update(double[] value, long timestamp) {
    if (!equalsLast(value)) {
      copyToLast(value);
      append(value, timestamp);
    }
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Value to record
   */
  public void update(double[] value) {
    update(value, 0);
  }

  /**
   * Gets whether there is a last value.
   *
   * <p>Note: the last value is local to this class instance and updated only with update(), not
   * append().
   *
   * @return True if last value exists, false otherwise.
   */
  public synchronized boolean hasLastValue() {
    return m_lastValue != null;
  }

  /**
   * Gets the last value.
   *
   * <p>Note: the last value is local to this class instance and updated only with update(), not
   * append().
   *
   * @return Last value, or false if none.
   */
  @SuppressWarnings("PMD.ReturnEmptyCollectionRatherThanNull")
  public synchronized double[] getLastValue() {
    if (m_lastValue == null) {
      return null;
    }
    return Arrays.copyOf(m_lastValue, m_lastValueLen);
  }

  private boolean equalsLast(double[] value) {
    if (m_lastValue == null || m_lastValueLen != value.length) {
      return false;
    }
    return Arrays.equals(m_lastValue, 0, value.length, value, 0, value.length);
  }

  private void copyToLast(double[] value) {
    if (m_lastValue == null || m_lastValue.length < value.length) {
      m_lastValue = Arrays.copyOf(value, value.length);
    } else {
      System.arraycopy(value, 0, m_lastValue, 0, value.length);
    }
    m_lastValueLen = value.length;
  }

  private double[] m_lastValue;
  private int m_lastValueLen;
}
