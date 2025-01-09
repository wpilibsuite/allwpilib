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
    m_immutable = struct.isImmutable();
    m_cloneable = struct.isCloneable();
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

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void update(T value, long timestamp) {
    synchronized (m_buf) {
      if (m_immutable || m_cloneable) {
        if (value.equals(m_lastValue)) {
          return;
        }
        try {
          if (m_immutable) {
            m_lastValue = value;
          } else {
            m_lastValue = m_buf.getStruct().clone(value);
          }
          ByteBuffer bb = m_buf.write(value);
          m_log.appendRaw(m_entry, bb, 0, bb.position(), timestamp);
          return;
        } catch (CloneNotSupportedException e) {
          // fall through
        }
      }
      doUpdate(m_buf.write(value), timestamp);
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
  public void update(T value) {
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
  public boolean hasLastValue() {
    synchronized (m_buf) {
      if (m_immutable) {
        return m_lastValue != null;
      } else if (m_cloneable && m_lastValue != null) {
        return true;
      }
      return m_lastValueBuf != null;
    }
  }

  /**
   * Gets the last value.
   *
   * <p>Note: the last value is local to this class instance and updated only with update(), not
   * append().
   *
   * @return Last value, or null if none.
   */
  public T getLastValue() {
    synchronized (m_buf) {
      if (m_immutable) {
        return m_lastValue;
      } else if (m_cloneable && m_lastValue != null) {
        try {
          return m_buf.getStruct().clone(m_lastValue);
        } catch (CloneNotSupportedException e) {
          // fall through
        }
      }
      if (m_lastValueBuf == null) {
        return null;
      }
      T val = m_buf.read(m_lastValueBuf);
      m_lastValueBuf.position(0);
      return val;
    }
  }

  private void doUpdate(ByteBuffer bb, long timestamp) {
    int len = bb.position();
    bb.limit(len);
    bb.position(0);
    if (m_lastValueBuf == null || !bb.equals(m_lastValueBuf)) {
      // update last value
      if (m_lastValueBuf == null || m_lastValueBuf.limit() < len) {
        m_lastValueBuf = ByteBuffer.allocate(len);
      }
      bb.get(m_lastValueBuf.array(), 0, len);
      bb.position(0);
      m_lastValueBuf.limit(len);

      // append to log
      m_log.appendRaw(m_entry, bb, 0, len, timestamp);
    }
  }

  private final StructBuffer<T> m_buf;
  private ByteBuffer m_lastValueBuf;
  private final boolean m_immutable;
  private final boolean m_cloneable;
  private T m_lastValue;
}
