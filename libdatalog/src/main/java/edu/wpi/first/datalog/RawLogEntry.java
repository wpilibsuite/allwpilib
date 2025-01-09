// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.nio.ByteBuffer;
import java.util.Arrays;

/** Log raw byte array values. */
public class RawLogEntry extends DataLogEntry {
  /** The data type for raw values. */
  public static final String kDataType = "raw";

  /**
   * Constructs a raw log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param type Data type
   * @param timestamp entry creation timestamp (0=now)
   */
  public RawLogEntry(DataLog log, String name, String metadata, String type, long timestamp) {
    super(log, name, type, metadata, timestamp);
  }

  /**
   * Constructs a raw log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param type Data type
   */
  public RawLogEntry(DataLog log, String name, String metadata, String type) {
    this(log, name, metadata, type, 0);
  }

  /**
   * Constructs a raw log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   */
  public RawLogEntry(DataLog log, String name, String metadata, long timestamp) {
    this(log, name, metadata, kDataType, timestamp);
  }

  /**
   * Constructs a raw log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param metadata metadata
   */
  public RawLogEntry(DataLog log, String name, String metadata) {
    this(log, name, metadata, 0);
  }

  /**
   * Constructs a raw log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param timestamp entry creation timestamp (0=now)
   */
  public RawLogEntry(DataLog log, String name, long timestamp) {
    this(log, name, "", timestamp);
  }

  /**
   * Constructs a raw log entry.
   *
   * @param log datalog
   * @param name name of the entry
   */
  public RawLogEntry(DataLog log, String name) {
    this(log, name, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record; will send entire array contents
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(byte[] value, long timestamp) {
    m_log.appendRaw(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record; will send entire array contents
   */
  public void append(byte[] value) {
    append(value, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Data to record
   * @param start Start position of data (in byte array)
   * @param len Length of data (must be less than or equal to value.length - offset)
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(byte[] value, int start, int len, long timestamp) {
    m_log.appendRaw(m_entry, value, start, len, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Data to record
   * @param start Start position of data (in byte array)
   * @param len Length of data (must be less than or equal to value.length - offset)
   */
  public void append(byte[] value, int start, int len) {
    append(value, start, len, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Data to record; will send from value.position() to value.limit()
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(ByteBuffer value, long timestamp) {
    m_log.appendRaw(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Data to record; will send from value.position() to value.limit()
   */
  public void append(ByteBuffer value) {
    append(value, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Data to record
   * @param start Start position of data (in value buffer)
   * @param len Length of data (must be less than or equal to value.length - offset)
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(ByteBuffer value, int start, int len, long timestamp) {
    m_log.appendRaw(m_entry, value, start, len, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Data to record
   * @param start Start position of data (in value buffer)
   * @param len Length of data (must be less than or equal to value.length - offset)
   */
  public void append(ByteBuffer value, int start, int len) {
    append(value, start, len, 0);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Value to record; will send entire array contents
   * @param timestamp Time stamp (0 to indicate now)
   */
  public synchronized void update(byte[] value, long timestamp) {
    if (!equalsLast(value, 0, value.length)) {
      copyToLast(value, 0, value.length);
      append(value, timestamp);
    }
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Value to record; will send entire array contents
   */
  public void update(byte[] value) {
    update(value, 0);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Data to record
   * @param start Start position of data (in byte array)
   * @param len Length of data (must be less than or equal to value.length - offset)
   * @param timestamp Time stamp (0 to indicate now)
   */
  public synchronized void update(byte[] value, int start, int len, long timestamp) {
    if (!equalsLast(value, start, len)) {
      copyToLast(value, start, len);
      append(value, start, len, timestamp);
    }
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Data to record
   * @param start Start position of data (in byte array)
   * @param len Length of data (must be less than or equal to value.length - offset)
   */
  public void update(byte[] value, int start, int len) {
    update(value, start, len, 0);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Data to record; will send from value.position() to value.limit()
   * @param timestamp Time stamp (0 to indicate now)
   */
  public synchronized void update(ByteBuffer value, long timestamp) {
    if (!equalsLast(value)) {
      int start = value.position();
      int len = value.limit() - start;
      copyToLast(value, start, len);
      append(value, start, len, timestamp);
    }
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Data to record; will send from value.position() to value.limit()
   */
  public void update(ByteBuffer value) {
    update(value, 0);
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Data to record
   * @param start Start position of data (in value buffer)
   * @param len Length of data (must be less than or equal to value.length - offset)
   * @param timestamp Time stamp (0 to indicate now)
   */
  public synchronized void update(ByteBuffer value, int start, int len, long timestamp) {
    if (!equalsLast(value, start, len)) {
      copyToLast(value, start, len);
      append(value, start, len, timestamp);
    }
  }

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Data to record
   * @param start Start position of data (in value buffer)
   * @param len Length of data (must be less than or equal to value.length - offset)
   */
  public void update(ByteBuffer value, int start, int len) {
    update(value, start, len, 0);
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
   * @return Last value, or null if none.
   */
  @SuppressWarnings("PMD.ReturnEmptyCollectionRatherThanNull")
  public synchronized byte[] getLastValue() {
    if (m_lastValue == null) {
      return null;
    }
    return Arrays.copyOf(m_lastValue.array(), m_lastValue.limit());
  }

  private boolean equalsLast(byte[] value, int start, int len) {
    if (m_lastValue == null || m_lastValue.limit() != len) {
      return false;
    }
    return Arrays.equals(m_lastValue.array(), 0, len, value, start, start + len);
  }

  @SuppressWarnings("PMD.SimplifyBooleanReturns")
  private boolean equalsLast(ByteBuffer value) {
    if (m_lastValue == null) {
      return false;
    }
    return value.equals(m_lastValue);
  }

  private boolean equalsLast(ByteBuffer value, int start, int len) {
    if (m_lastValue == null || m_lastValue.limit() != len) {
      return false;
    }
    int origpos = value.position();
    value.position(start);
    int origlimit = value.limit();
    value.limit(start + len);
    boolean eq = value.equals(m_lastValue);
    value.position(origpos);
    value.limit(origlimit);
    return eq;
  }

  private void copyToLast(byte[] value, int start, int len) {
    if (m_lastValue == null || m_lastValue.limit() < len) {
      m_lastValue = ByteBuffer.allocate(len);
    }
    System.arraycopy(value, start, m_lastValue.array(), 0, len);
    m_lastValue.limit(len);
  }

  private void copyToLast(ByteBuffer value, int start, int len) {
    if (m_lastValue == null || m_lastValue.limit() < len) {
      m_lastValue = ByteBuffer.allocate(len);
    }
    int origpos = value.position();
    value.position(start);
    value.get(m_lastValue.array(), 0, len);
    value.position(origpos);
    m_lastValue.limit(len);
  }

  private ByteBuffer m_lastValue;
}
