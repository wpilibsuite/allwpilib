// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructBuffer;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.util.Arrays;
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
    m_immutable = struct.isImmutable();
    m_cloneable = struct.isCloneable();
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
    synchronized (m_buf) {
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
    synchronized (m_buf) {
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
    synchronized (m_buf) {
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

  /**
   * Updates the last value and appends a record to the log if it has changed.
   *
   * <p>Note: the last value is local to this class instance; using update() with two instances
   * pointing to the same underlying log entry name will likely result in unexpected results.
   *
   * @param value Value to record
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void update(T[] value, long timestamp) {
    synchronized (m_buf) {
      if (m_immutable || m_cloneable) {
        if (m_lastValue != null
            && m_lastValueLen == value.length
            && Arrays.equals(m_lastValue, 0, value.length, value, 0, value.length)) {
          return;
        }
        try {
          copyToLast(value);
          ByteBuffer bb = m_buf.writeArray(value);
          m_log.appendRaw(m_entry, bb, 0, bb.position(), timestamp);
          return;
        } catch (CloneNotSupportedException e) {
          // fall through
        }
      }
      doUpdate(m_buf.writeArray(value), timestamp);
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
  public void update(T[] value) {
    update(value, 0);
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
  public void update(Collection<T> value, long timestamp) {
    synchronized (m_buf) {
      if (m_immutable || m_cloneable) {
        if (equalsLast(value)) {
          return;
        }
        try {
          copyToLast(value);
          ByteBuffer bb = m_buf.writeArray(value);
          m_log.appendRaw(m_entry, bb, 0, bb.position(), timestamp);
          return;
        } catch (CloneNotSupportedException e) {
          // fall through
        }
      }
      doUpdate(m_buf.writeArray(value), timestamp);
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
  public void update(Collection<T> value) {
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
  @SuppressWarnings("PMD.ReturnEmptyCollectionRatherThanNull")
  public T[] getLastValue() {
    synchronized (m_buf) {
      if (m_immutable) {
        if (m_lastValue == null) {
          return null;
        }
        return Arrays.copyOf(m_lastValue, m_lastValueLen);
      } else if (m_cloneable && m_lastValue != null) {
        try {
          return cloneArray(m_lastValue, m_lastValueLen);
        } catch (CloneNotSupportedException e) {
          // fall through
        }
      }
      if (m_lastValueBuf == null) {
        return null;
      }
      T[] val = m_buf.readArray(m_lastValueBuf);
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

  private boolean equalsLast(Collection<T> arr) {
    if (m_lastValue == null) {
      return false;
    }
    if (m_lastValueLen != arr.size()) {
      return false;
    }
    int i = 0;
    for (T elem : arr) {
      if (!m_lastValue[i].equals(elem)) {
        return false;
      }
      i++;
    }
    return true;
  }

  private T[] cloneArray(T[] in, int len) throws CloneNotSupportedException {
    Struct<T> s = m_buf.getStruct();
    @SuppressWarnings("unchecked")
    T[] arr = (T[]) Array.newInstance(s.getTypeClass(), len);
    for (int i = 0; i < len; i++) {
      arr[i] = s.clone(in[i]);
    }
    return arr;
  }

  private T[] cloneArray(Collection<T> in) throws CloneNotSupportedException {
    Struct<T> s = m_buf.getStruct();
    @SuppressWarnings("unchecked")
    T[] arr = (T[]) Array.newInstance(s.getTypeClass(), in.size());
    int i = 0;
    for (T elem : in) {
      arr[i++] = s.clone(elem);
    }
    return arr;
  }

  private void copyToLast(T[] value) throws CloneNotSupportedException {
    if (m_lastValue == null || m_lastValue.length < value.length) {
      if (m_immutable) {
        m_lastValue = Arrays.copyOf(value, value.length);
      } else {
        m_lastValue = cloneArray(value, value.length);
      }
    } else {
      if (m_immutable) {
        System.arraycopy(value, 0, m_lastValue, 0, value.length);
      } else {
        Struct<T> s = m_buf.getStruct();
        for (int i = 0; i < value.length; i++) {
          m_lastValue[i] = s.clone(value[i]);
        }
      }
    }
    m_lastValueLen = value.length;
  }

  private void copyToLast(Collection<T> value) throws CloneNotSupportedException {
    if (m_lastValue == null || m_lastValue.length < value.size()) {
      if (m_immutable) {
        Struct<T> s = m_buf.getStruct();
        @SuppressWarnings("unchecked")
        T[] arr = (T[]) Array.newInstance(s.getTypeClass(), value.size());
        int i = 0;
        for (T elem : value) {
          arr[i++] = elem;
        }
      } else {
        m_lastValue = cloneArray(value);
      }
    } else {
      Struct<T> s = m_buf.getStruct();
      int i = 0;
      for (T elem : value) {
        m_lastValue[i++] = m_immutable ? elem : s.clone(elem);
      }
    }
    m_lastValueLen = value.size();
  }

  private final StructBuffer<T> m_buf;
  private ByteBuffer m_lastValueBuf;
  private final boolean m_immutable;
  private final boolean m_cloneable;
  private T[] m_lastValue;
  private int m_lastValueLen;
}
