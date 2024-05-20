// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructBuffer;
import java.nio.ByteBuffer;

/**
 * Log struct-encoded values.
 *
 * @param <T> value class
 */
public final class StructLogEntry<T> extends DataLogEntry {
  private StructLogEntry(
      DataLog log, String name, Struct<T> struct, String metadata, long timestamp) {
    super(log, name, struct.getTypeString(), metadata, timestamp);
    m_buf = StructBuffer.create(struct);
    log.addSchema(struct, timestamp);
  }

  /**
   * Creates a struct-encoded log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   * @return StructLogEntry
   */
  public static <T> StructLogEntry<T> create(
      DataLog log, String name, Struct<T> struct, String metadata, long timestamp) {
    return new StructLogEntry<>(log, name, struct, metadata, timestamp);
  }

  /**
   * Creates a struct-encoded log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @param metadata metadata
   * @return StructLogEntry
   */
  public static <T> StructLogEntry<T> create(
      DataLog log, String name, Struct<T> struct, String metadata) {
    return create(log, name, struct, metadata, 0);
  }

  /**
   * Creates a struct-encoded log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @param timestamp entry creation timestamp (0=now)
   * @return StructLogEntry
   */
  public static <T> StructLogEntry<T> create(
      DataLog log, String name, Struct<T> struct, long timestamp) {
    return create(log, name, struct, "", timestamp);
  }

  /**
   * Creates a struct-encoded log entry.
   *
   * @param <T> value class (inferred from struct)
   * @param log datalog
   * @param name name of the entry
   * @param struct struct serialization implementation
   * @return StructLogEntry
   */
  public static <T> StructLogEntry<T> create(DataLog log, String name, Struct<T> struct) {
    return create(log, name, struct, 0);
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void append(T value, long timestamp) {
    synchronized (m_buf) {
      ByteBuffer bb = m_buf.write(value);
      m_log.appendRaw(m_entry, bb, 0, bb.position(), timestamp);
    }
  }

  /**
   * Appends a record to the log.
   *
   * @param value Value to record
   */
  public void append(T value) {
    append(value, 0);
  }

  private final StructBuffer<T> m_buf;
}
