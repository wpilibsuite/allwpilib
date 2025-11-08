// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import org.wpilib.util.struct.StructBuffer;

/**
 * NetworkTables struct-encoded value implementation.
 *
 * @param <T> value class
 */
@SuppressWarnings("PMD.ArrayIsStoredDirectly")
final class StructArrayEntryImpl<T> extends EntryBase implements StructArrayEntry<T> {
  /**
   * Constructor.
   *
   * @param topic Topic
   * @param handle Native handle
   * @param defaultValue Default value for get()
   */
  StructArrayEntryImpl(
      StructArrayTopic<T> topic,
      StructBuffer<T> buf,
      int handle,
      T[] defaultValue,
      boolean schemaPublished) {
    super(handle);
    m_topic = topic;
    m_defaultValue = defaultValue;
    m_buf = buf;
    m_schemaPublished = schemaPublished;
  }

  @Override
  public StructArrayTopic<T> getTopic() {
    return m_topic;
  }

  @Override
  public T[] get() {
    return fromRaw(NetworkTablesJNI.getRaw(m_handle, m_emptyRaw), m_defaultValue);
  }

  @Override
  public T[] get(T[] defaultValue) {
    return fromRaw(NetworkTablesJNI.getRaw(m_handle, m_emptyRaw), defaultValue);
  }

  @Override
  public TimestampedObject<T[]> getAtomic() {
    return fromRaw(NetworkTablesJNI.getAtomicRaw(m_handle, m_emptyRaw), m_defaultValue);
  }

  @Override
  public TimestampedObject<T[]> getAtomic(T[] defaultValue) {
    return fromRaw(NetworkTablesJNI.getAtomicRaw(m_handle, m_emptyRaw), defaultValue);
  }

  @Override
  public TimestampedObject<T[]>[] readQueue() {
    TimestampedRaw[] raw = NetworkTablesJNI.readQueueRaw(m_handle);
    @SuppressWarnings("unchecked")
    TimestampedObject<T[]>[] arr = (TimestampedObject<T[]>[]) new TimestampedObject<?>[raw.length];
    int err = 0;
    for (int i = 0; i < raw.length; i++) {
      arr[i] = fromRaw(raw[i], null);
      if (arr[i].value == null) {
        err++;
      }
    }

    // discard bad values
    if (err > 0) {
      @SuppressWarnings("unchecked")
      TimestampedObject<T[]>[] newArr =
          (TimestampedObject<T[]>[]) new TimestampedObject<?>[raw.length - err];
      int i = 0;
      for (TimestampedObject<T[]> e : arr) {
        if (e.value != null) {
          arr[i] = e;
          i++;
        }
      }
      arr = newArr;
    }

    return arr;
  }

  @Override
  public T[][] readQueueValues() {
    byte[][] raw = NetworkTablesJNI.readQueueValuesRaw(m_handle);
    @SuppressWarnings("unchecked")
    T[][] arr = (T[][]) Array.newInstance(Array.class, raw.length);
    int err = 0;
    for (int i = 0; i < raw.length; i++) {
      arr[i] = fromRaw(raw[i], null);
      if (arr[i] == null) {
        err++;
      }
    }

    // discard bad values
    if (err > 0) {
      @SuppressWarnings("unchecked")
      T[][] newArr = (T[][]) Array.newInstance(Array.class, raw.length - err);
      int i = 0;
      for (T[] e : arr) {
        if (e != null) {
          arr[i] = e;
          i++;
        }
      }
      arr = newArr;
    }

    return arr;
  }

  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  @Override
  public void set(T[] value, long time) {
    try {
      synchronized (m_buf) {
        if (!m_schemaPublished) {
          m_schemaPublished = true;
          m_topic.getInstance().addSchema(m_buf.getStruct());
        }
        ByteBuffer bb = m_buf.writeArray(value);
        NetworkTablesJNI.setRaw(m_handle, time, bb, 0, bb.position());
      }
    } catch (RuntimeException e) {
      // ignore
    }
  }

  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  @Override
  public void setDefault(T[] value) {
    try {
      synchronized (m_buf) {
        if (!m_schemaPublished) {
          m_schemaPublished = true;
          m_topic.getInstance().addSchema(m_buf.getStruct());
        }
        ByteBuffer bb = m_buf.writeArray(value);
        NetworkTablesJNI.setDefaultRaw(m_handle, 0, bb, 0, bb.position());
      }
    } catch (RuntimeException e) {
      // ignore
    }
  }

  @Override
  public void unpublish() {
    NetworkTablesJNI.unpublish(m_handle);
  }

  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  private T[] fromRaw(byte[] raw, T[] defaultValue) {
    if (raw.length == 0) {
      return defaultValue;
    }
    try {
      synchronized (m_buf) {
        return m_buf.readArray(raw);
      }
    } catch (RuntimeException e) {
      return defaultValue;
    }
  }

  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  private TimestampedObject<T[]> fromRaw(TimestampedRaw raw, T[] defaultValue) {
    if (raw.value.length == 0) {
      return new TimestampedObject<>(0, 0, defaultValue);
    }
    try {
      synchronized (m_buf) {
        return new TimestampedObject<>(raw.timestamp, raw.serverTime, m_buf.readArray(raw.value));
      }
    } catch (RuntimeException e) {
      return new TimestampedObject<>(0, 0, defaultValue);
    }
  }

  private final StructArrayTopic<T> m_topic;
  private final T[] m_defaultValue;
  private final StructBuffer<T> m_buf;
  private boolean m_schemaPublished;
  private static final byte[] m_emptyRaw = new byte[] {};
}
