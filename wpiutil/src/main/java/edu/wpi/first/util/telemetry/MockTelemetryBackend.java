// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.BiConsumer;

/** A telemetry backend that saves logged data to an array for unit testing purposes. */
public class MockTelemetryBackend implements TelemetryBackend {
  /** Value for keepDuplicate() action. */
  public record KeepDuplicateValue(boolean value) {}

  /** Value for setProperty() action. */
  public record SetPropertyValue(String key, String value) {}

  /** Value for logStruct() action. */
  public record LogStructValue<T>(T value, Struct<T> struct) {}

  /** Value for logProtobuf() action. */
  public record LogProtobufValue<T>(T value, Protobuf<T, ?> protobuf) {}

  /** Value for logStructArray() action. */
  public record LogStructArrayValue<T>(T[] value, Struct<T> struct) {}

  /** Value for logString() action. */
  public record LogStringValue(String value, String typeString) {}

  /** Value for logRaw() action. */
  public record LogRawValue(byte[] value, String typeString) {}

  /** A logged action. */
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
    m_actions.clear();
  }

  @Override
  public void close() {
    synchronized (m_entries) {
      m_entries.clear();
    }
  }

  @Override
  public TelemetryEntry getEntry(String path) {
    synchronized (m_entries) {
      return m_entries.computeIfAbsent(path, k -> new Entry(k));
    }
  }

  @Override
  public void setReportWarning(BiConsumer<String, StackTraceElement[]> func) {}

  private void log(String path, Object value) {
    synchronized (m_actions) {
      m_actions.add(new Action(path, value));
    }
  }

  private final class Entry implements TelemetryEntry {
    Entry(String path) {
      m_path = path;
    }

    @Override
    public void keepDuplicates() {
      log(m_path, new KeepDuplicateValue(true));
    }

    @Override
    public void setProperty(String key, String value) {
      log(m_path, new SetPropertyValue(key, value));
    }

    @Override
    public <T> void logStruct(T value, Struct<T> struct) {
      if (struct.isImmutable()) {
        // log it directly
        log(m_path, new LogStructValue<T>(value, struct));
      } else if (struct.isCloneable()) {
        T clonedValue;
        try {
          clonedValue = struct.clone(value);
        } catch (CloneNotSupportedException e) {
          throw new UnsupportedOperationException(
              "struct.isCloneable() returned true, but clone() raised exception");
        }
        log(m_path, new LogStructValue<T>(clonedValue, struct));
      } else {
        // log it directly, but warn
        System.err.println(
            "warning: logging non-immutable and non-cloneable struct to '" + m_path + "'");
        log(m_path, new LogStructValue<T>(value, struct));
      }
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {
      if (proto.isImmutable()) {
        // log it directly
        log(m_path, new LogProtobufValue<T>(value, proto));
      } else if (proto.isCloneable()) {
        T clonedValue;
        try {
          clonedValue = proto.clone(value);
        } catch (CloneNotSupportedException e) {
          throw new UnsupportedOperationException(
              "proto.isCloneable() returned true, but clone() raised exception");
        }
        log(m_path, new LogProtobufValue<T>(clonedValue, proto));
      } else {
        // log it directly, but warn
        System.err.println(
            "warning: logging non-immutable and non-cloneable proto to '" + m_path + "'");
        log(m_path, new LogProtobufValue<T>(value, proto));
      }
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<T> struct) {
      if (struct.isImmutable()) {
        // log it directly
        log(m_path, new LogStructArrayValue<T>(value.clone(), struct));
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
        log(m_path, new LogStructArrayValue<T>(clonedArray, struct));
      } else {
        // log it directly, but warn
        System.err.println(
            "warning: logging non-immutable and non-cloneable struct to '" + m_path + "'");
        log(m_path, new LogStructArrayValue<T>(value.clone(), struct));
      }
    }

    @Override
    public void logBoolean(boolean value) {
      log(m_path, value);
    }

    @Override
    public void logShort(short value) {
      log(m_path, value);
    }

    @Override
    public void logInt(int value) {
      log(m_path, value);
    }

    @Override
    public void logLong(long value) {
      log(m_path, value);
    }

    @Override
    public void logFloat(float value) {
      log(m_path, value);
    }

    @Override
    public void logDouble(double value) {
      log(m_path, value);
    }

    @Override
    public void logString(String value, String typeString) {
      log(m_path, new LogStringValue(value, typeString));
    }

    @Override
    public void logBooleanArray(boolean[] value) {
      log(m_path, value.clone());
    }

    @Override
    public void logShortArray(short[] value) {
      log(m_path, value.clone());
    }

    @Override
    public void logIntArray(int[] value) {
      log(m_path, value.clone());
    }

    @Override
    public void logLongArray(long[] value) {
      log(m_path, value.clone());
    }

    @Override
    public void logFloatArray(float[] value) {
      log(m_path, value.clone());
    }

    @Override
    public void logDoubleArray(double[] value) {
      log(m_path, value.clone());
    }

    @Override
    public void logStringArray(String[] value) {
      log(m_path, value.clone());
    }

    @Override
    public void logRaw(byte[] value, String typeString) {
      log(m_path, new LogRawValue(value.clone(), typeString));
    }

    private final String m_path;
  }
}
