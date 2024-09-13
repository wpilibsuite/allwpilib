// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.nio.ByteBuffer;

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
   * @param value Data to record; will send from value.position() to value.capacity()
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(ByteBuffer value, long timestamp) {
    m_log.appendRaw(m_entry, value, timestamp);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Data to record; will send from value.position() to value.capacity()
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
}
