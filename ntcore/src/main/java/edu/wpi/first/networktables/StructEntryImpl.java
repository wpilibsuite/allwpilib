// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.struct.StructBuffer;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;

/**
 * NetworkTables struct-encoded value implementation.
 *
 * @param <T> value class
 */
final class StructEntryImpl<T> extends EntryBase implements StructEntry<T> {
  /**
   * Constructor.
   *
   * @param topic Topic
   * @param handle Native handle
   * @param defaultValue Default value for get()
   */
  StructEntryImpl(
      StructTopic<T> topic,
      StructBuffer<T> buf,
      int handle,
      T defaultValue,
      boolean schemaPublished) {
    super(handle);
    m_topic = topic;
    m_defaultValue = defaultValue;
    m_buf = buf;
    m_schemaPublished = schemaPublished;
  }

  @Override
  public StructTopic<T> getTopic() {
    return m_topic;
  }

  @Override
  public T get() {
    return fromRaw(NetworkTablesJNI.getRaw(m_handle, m_emptyRaw), m_defaultValue);
  }

  @Override
  public T get(T defaultValue) {
    return fromRaw(NetworkTablesJNI.getRaw(m_handle, m_emptyRaw), defaultValue);
  }

  @Override
  public boolean getInto(T out) {
    byte[] raw = NetworkTablesJNI.getRaw(m_handle, m_emptyRaw);
    if (raw.length == 0) {
      return false;
    }
    synchronized (m_buf) {
      m_buf.readInto(out, raw);
      return true;
    }
  }

  @Override
  public TimestampedObject<T> getAtomic() {
    return fromRaw(NetworkTablesJNI.getAtomicRaw(m_handle, m_emptyRaw), m_defaultValue);
  }

  @Override
  public TimestampedObject<T> getAtomic(T defaultValue) {
    return fromRaw(NetworkTablesJNI.getAtomicRaw(m_handle, m_emptyRaw), defaultValue);
  }

  @Override
  public TimestampedObject<T>[] readQueue() {
    TimestampedRaw[] raw = NetworkTablesJNI.readQueueRaw(m_handle);
    @SuppressWarnings("unchecked")
    TimestampedObject<T>[] arr = (TimestampedObject<T>[]) new TimestampedObject<?>[raw.length];
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
      TimestampedObject<T>[] newArr =
          (TimestampedObject<T>[]) new TimestampedObject<?>[raw.length - err];
      int i = 0;
      for (TimestampedObject<T> e : arr) {
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
  public T[] readQueueValues() {
    byte[][] raw = NetworkTablesJNI.readQueueValuesRaw(m_handle);
    @SuppressWarnings("unchecked")
    T[] arr = (T[]) Array.newInstance(m_topic.getStruct().getTypeClass(), raw.length);
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
      T[] newArr = (T[]) Array.newInstance(m_topic.getStruct().getTypeClass(), raw.length - err);
      int i = 0;
      for (T e : arr) {
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
  public void set(T value, long time) {
    try {
      synchronized (m_buf) {
        if (!m_schemaPublished) {
          m_schemaPublished = true;
          m_topic.getInstance().addSchema(m_buf.getStruct());
        }
        ByteBuffer bb = m_buf.write(value);
        NetworkTablesJNI.setRaw(m_handle, time, bb, 0, bb.position());
      }
    } catch (RuntimeException e) {
      // ignore
    }
  }

  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  @Override
  public void setDefault(T value) {
    try {
      synchronized (m_buf) {
        if (!m_schemaPublished) {
          m_schemaPublished = true;
          m_topic.getInstance().addSchema(m_buf.getStruct());
        }
        ByteBuffer bb = m_buf.write(value);
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
  private T fromRaw(byte[] raw, T defaultValue) {
    if (raw.length == 0) {
      return defaultValue;
    }
    try {
      synchronized (m_buf) {
        return m_buf.read(raw);
      }
    } catch (RuntimeException e) {
      return defaultValue;
    }
  }

  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  private TimestampedObject<T> fromRaw(TimestampedRaw raw, T defaultValue) {
    if (raw.value.length == 0) {
      return new TimestampedObject<>(0, 0, defaultValue);
    }
    try {
      synchronized (m_buf) {
        return new TimestampedObject<>(raw.timestamp, raw.serverTime, m_buf.read(raw.value));
      }
    } catch (RuntimeException e) {
      return new TimestampedObject<>(0, 0, defaultValue);
    }
  }

  private final StructTopic<T> m_topic;
  private final T m_defaultValue;
  private final StructBuffer<T> m_buf;
  private boolean m_schemaPublished;
  private static final byte[] m_emptyRaw = new byte[] {};
}
