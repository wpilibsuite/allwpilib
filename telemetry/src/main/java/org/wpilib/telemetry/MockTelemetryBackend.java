// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/** A telemetry backend that saves logged data to an array for unit testing purposes. */
public class MockTelemetryBackend implements TelemetryBackend {
  /** Constructor. */
  public MockTelemetryBackend() {}

  /**
   * Value for keepDuplicate() action.
   *
   * @param value value passed to keepDuplicate()
   */
  public record KeepDuplicateValue(boolean value) {}

  /**
   * Value for setProperty() action.
   *
   * @param key key passed to setProperty()
   * @param value value passed to setProperty()
   */
  public record SetPropertyValue(String key, String value) {}

  /**
   * Value for logStruct() action.
   *
   * @param <T> data type
   * @param value value passed to logStruct()
   * @param struct struct passed to logStruct()
   */
  public record LogStructValue<T>(T value, Struct<T> struct) {}

  /**
   * Value for logProtobuf() action.
   *
   * @param <T> data type
   * @param value value passed to logProtobuf()
   * @param protobuf protobuf passed to logProtobuf()
   */
  public record LogProtobufValue<T>(T value, Protobuf<T, ?> protobuf) {}

  /**
   * Value for logStructArray() action.
   *
   * @param <T> data type
   * @param value value passed to logStructArray()
   * @param struct struct passed to logStructArray()
   */
  public record LogStructArrayValue<T>(T[] value, Struct<T> struct) {}

  /**
   * Value for logString() action.
   *
   * @param value value passed to logString()
   * @param typeString type string passed to logString()
   */
  public record LogStringValue(String value, String typeString) {}

  /**
   * Value for logRaw() action.
   *
   * @param value value passed to logRaw()
   * @param typeString type string passed to logRaw()
   */
  public record LogRawValue(byte[] value, String typeString) {}

  /**
   * A logged action.
   *
   * @param path logged path
   * @param value logged value
   */
  public record Action(String path, Object value) {}

  private final Map<String, Entry> m_entries = new HashMap<>();
  private final List<Action> m_actions = new ArrayList<>();

  /**
   * Get logged actions.
   *
   * @return list of actions
   */
  public List<Action> getActions() {
    return m_actions;
  }

  /** Clear logged actions. */
  public void clear() {
    synchronized (this) {
      for (Entry entry : m_entries.values()) {
        entry.m_last = -1;
      }
      m_actions.clear();
    }
  }

  /**
   * Get the last action for a particular path.
   *
   * @param path path
   * @return Action, or null if no update to that path
   */
  public Action getLastAction(String path) {
    synchronized (this) {
      Entry entry = m_entries.get(path);
      if (entry == null || entry.m_last == -1) {
        return null;
      }
      return m_actions.get(entry.m_last);
    }
  }

  /**
   * Get the last action value for a particular path.
   *
   * @param <T> data type
   * @param path path
   * @param cls class for desired data type
   * @return Value, or null if no update to that path
   */
  public <T> T getLastValue(String path, Class<T> cls) {
    Action action = getLastAction(path);
    if (action == null || !action.value.getClass().equals(cls)) {
      return null;
    }
    @SuppressWarnings("unchecked")
    T result = (T) action.value;
    return result;
  }

  @Override
  public void close() {
    synchronized (this) {
      m_entries.clear();
    }
  }

  @Override
  public TelemetryEntry getEntry(String path) {
    synchronized (this) {
      return m_entries.computeIfAbsent(path, k -> new Entry(k));
    }
  }

  private void log(Entry entry, Object value) {
    synchronized (this) {
      entry.m_last = m_actions.size();
      m_actions.add(new Action(entry.m_path, value));
    }
  }

  private final class Entry implements TelemetryEntry {
    Entry(String path) {
      m_path = path;
    }

    @Override
    public void keepDuplicates() {
      log(this, new KeepDuplicateValue(true));
    }

    @Override
    public void setProperty(String key, String value) {
      log(this, new SetPropertyValue(key, value));
    }

    @Override
    public <T> void logStruct(T value, Struct<T> struct) {
      if (struct.isImmutable()) {
        // log it directly
        log(this, new LogStructValue<T>(value, struct));
      } else if (struct.isCloneable()) {
        T clonedValue;
        try {
          clonedValue = struct.clone(value);
        } catch (CloneNotSupportedException e) {
          throw new UnsupportedOperationException(
              "struct.isCloneable() returned true, but clone() raised exception");
        }
        log(this, new LogStructValue<T>(clonedValue, struct));
      } else {
        // log it directly, but warn
        TelemetryRegistry.reportWarning(m_path, "logging non-immutable and non-cloneable struct");
        log(this, new LogStructValue<T>(value, struct));
      }
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {
      if (proto.isImmutable()) {
        // log it directly
        log(this, new LogProtobufValue<T>(value, proto));
      } else if (proto.isCloneable()) {
        T clonedValue;
        try {
          clonedValue = proto.clone(value);
        } catch (CloneNotSupportedException e) {
          throw new UnsupportedOperationException(
              "proto.isCloneable() returned true, but clone() raised exception");
        }
        log(this, new LogProtobufValue<T>(clonedValue, proto));
      } else {
        // log it directly, but warn
        TelemetryRegistry.reportWarning(m_path, "logging non-immutable and non-cloneable proto");
        log(this, new LogProtobufValue<T>(value, proto));
      }
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<T> struct) {
      if (struct.isImmutable()) {
        // log it directly
        log(this, new LogStructArrayValue<T>(value.clone(), struct));
      } else if (struct.isCloneable()) {
        @SuppressWarnings("unchecked")
        T[] clonedArray = (T[]) Array.newInstance(struct.getClass(), value.length);
        try {
          for (int i = 0; i < value.length; i++) {
            clonedArray[i] = struct.clone(value[i]);
          }
        } catch (CloneNotSupportedException e) {
          throw new UnsupportedOperationException(
              "struct.isCloneable() returned true, but clone() raised exception");
        }
        log(this, new LogStructArrayValue<T>(clonedArray, struct));
      } else {
        // log it directly, but warn
        TelemetryRegistry.reportWarning(m_path, "logging non-immutable and non-cloneable struct");
        log(this, new LogStructArrayValue<T>(value.clone(), struct));
      }
    }

    @Override
    public void logBoolean(boolean value) {
      log(this, value);
    }

    @Override
    public void logShort(short value) {
      log(this, value);
    }

    @Override
    public void logInt(int value) {
      log(this, value);
    }

    @Override
    public void logLong(long value) {
      log(this, value);
    }

    @Override
    public void logFloat(float value) {
      log(this, value);
    }

    @Override
    public void logDouble(double value) {
      log(this, value);
    }

    @Override
    public void logString(String value, String typeString) {
      log(this, new LogStringValue(value, typeString));
    }

    @Override
    public void logBooleanArray(boolean[] value) {
      log(this, value.clone());
    }

    @Override
    public void logShortArray(short[] value) {
      log(this, value.clone());
    }

    @Override
    public void logIntArray(int[] value) {
      log(this, value.clone());
    }

    @Override
    public void logLongArray(long[] value) {
      log(this, value.clone());
    }

    @Override
    public void logFloatArray(float[] value) {
      log(this, value.clone());
    }

    @Override
    public void logDoubleArray(double[] value) {
      log(this, value.clone());
    }

    @Override
    public void logStringArray(String[] value) {
      log(this, value.clone());
    }

    @Override
    public void logRaw(byte[] value, String typeString) {
      log(this, new LogRawValue(value.clone(), typeString));
    }

    private final String m_path;
    private int m_last = -1;
  }
}
