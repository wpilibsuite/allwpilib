// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructBuffer;
import java.nio.ByteBuffer;
import java.util.Collection;

/**
 * Log struct-encoded array values.
 *
 * @param <T> value class
 */
public final class StructArrayLogEntry<T> extends DataLogEntry {
  private StructArrayLogEntry(
      DataLog log, String name, Struct<T> struct, String metadata, long timestamp) {
    super(log, name, struct.getTypeString() + "[]", metadata, timestamp);
    m_buf = StructBuffer.create(struct);
    log.addSchema(struct, timestamp);
  }

  /**
   * Creates a struct-encoded array log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   * @return StructArrayLogEntry
   */
  public static <T> StructArrayLogEntry<T> create(
      DataLog log, String name, Struct<T> struct, String metadata, long timestamp) {
    return new StructArrayLogEntry<>(log, name, struct, metadata, timestamp);
  }

  /**
   * Creates a struct-encoded array log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @param metadata metadata
   * @return StructArrayLogEntry
   */
  public static <T> StructArrayLogEntry<T> create(
      DataLog log, String name, Struct<T> struct, String metadata) {
    return create(log, name, struct, metadata, 0);
  }

  /**
   * Creates a struct-encoded array log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @param timestamp entry creation timestamp (0=now)
   * @return StructArrayLogEntry
   */
  public static <T> StructArrayLogEntry<T> create(
      DataLog log, String name, Struct<T> struct, long timestamp) {
    return create(log, name, struct, "", timestamp);
  }

  /**
   * Creates a struct-encoded array log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @return StructArrayLogEntry
   */
  public static <T> StructArrayLogEntry<T> create(DataLog log, String name, Struct<T> struct) {
    return create(log, name, struct, 0);
  }

  /**
   * Ensures sufficient buffer space is available for the given number of elements.
   *
   * @param nelem number of elements
   */
  public void reserve(int nelem) {
    synchronized (this) {
      m_buf.reserve(nelem);
    }
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(T[] value, long timestamp) {
    synchronized (this) {
      ByteBuffer bb = m_buf.writeArray(value);
      m_log.appendRaw(m_entry, bb, 0, bb.position(), timestamp);
    }
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(T[] value) {
    append(value, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(Collection<T> value, long timestamp) {
    synchronized (this) {
      ByteBuffer bb = m_buf.writeArray(value);
      m_log.appendRaw(m_entry, bb, 0, bb.position(), timestamp);
    }
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(Collection<T> value) {
    append(value, 0);
  }

  private final StructBuffer<T> m_buf;
}
