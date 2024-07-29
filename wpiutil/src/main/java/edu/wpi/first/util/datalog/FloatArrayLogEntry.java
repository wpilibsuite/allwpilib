// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.util.Arrays;

/** Log array of float values. */
public class FloatArrayLogEntry extends DataLogEntry {
  /** The data type for float array values. */
  public static final String kDataType = "float[]";

  /**
   * Constructs a float array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   */
  public FloatArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, kDataType, metadata, timestamp);
  }

  /**
   * Constructs a float array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   */
  public FloatArrayLogEntry(DataLog log, String name, String metadata) {
    this(log, name, metadata, 0);
  }

  /**
   * Constructs a float array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param timestamp entry creation timestamp (0=now)
   */
  public FloatArrayLogEntry(DataLog log, String name, long timestamp) {
    this(log, name, "", timestamp);
  }

  /**
   * Constructs a float array log entry.
   *
   * @param log datalog
   * @param name name of the entry
   */
  public FloatArrayLogEntry(DataLog log, String name) {
    this(log, name, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(float[] value, long timestamp) {
    m_log.appendFloatArray(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(float[] value) {
    m_log.appendFloatArray(m_entry, value, 0);
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
  public synchronized void update(float[] value, long timestamp) {
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
  public void update(float[] value) {
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
  public synchronized float[] getLastValue() {
    if (m_lastValue == null) {
      return null;
    }
    return Arrays.copyOf(m_lastValue, m_lastValueLen);
  }

  private boolean equalsLast(float[] value) {
    if (m_lastValue == null || m_lastValueLen != value.length) {
      return false;
    }
    return Arrays.equals(m_lastValue, 0, value.length, value, 0, value.length);
  }

  private void copyToLast(float[] value) {
    if (m_lastValue == null || m_lastValue.length < value.length) {
      m_lastValue = Arrays.copyOf(value, value.length);
    } else {
      System.arraycopy(value, 0, m_lastValue, 0, value.length);
    }
    m_lastValueLen = value.length;
  }

  private float[] m_lastValue;
  private int m_lastValueLen;
}
