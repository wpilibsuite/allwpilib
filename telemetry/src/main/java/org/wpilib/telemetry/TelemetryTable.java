// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;

/**
 * Telemetry sends information from the robot program to dashboards, debug tools, or log files.
 *
 * <p>For more advanced use cases, use the NetworkTables or DataLog APIs.
 */
public final class TelemetryTable {
  private record StaticFieldLookup(Object value, String warning) {}

  private static final ClassValue<StaticFieldLookup> s_structLookupCache =
      new ClassValue<>() {
        @Override
        protected StaticFieldLookup computeValue(Class<?> type) {
          return getStaticField(type, "struct");
        }
      };

  private static final ClassValue<StaticFieldLookup> s_protoLookupCache =
      new ClassValue<>() {
        @Override
        protected StaticFieldLookup computeValue(Class<?> type) {
          return getStaticField(type, "proto");
        }
      };

  private final String m_path;
  private final ConcurrentMap<String, TelemetryTable> m_tablesMap = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, TelemetryEntry> m_entriesMap = new ConcurrentHashMap<>();
  private String m_type;

  private static StaticFieldLookup getStaticField(Class<?> type, String fieldName) {
    try {
      return new StaticFieldLookup(type.getField(fieldName).get(null), null);
    } catch (NoSuchFieldException e) {
      return new StaticFieldLookup(
          null, "could not get " + fieldName + " field for " + type.getName());
    } catch (IllegalAccessException e) {
      return new StaticFieldLookup(
          null, "could not access " + fieldName + " field for " + type.getName());
    }
  }

  /**
   * Constructs a telemetry table.
   *
   * @param path path with trailing "/".
   */
  TelemetryTable(String path) {
    m_path = path;
  }

  /** Clears the table's cached entries. */
  void reset() {
    m_tablesMap.clear();
    m_entriesMap.clear();
  }

  /**
   * Gets the table path.
   *
   * @return path with trailing "/"
   */
  public String getPath() {
    return m_path;
  }

  /**
   * Sets the table type. TelemetryLoggable implementations can use this function to communicate the
   * type of data in the table. Callers should check the return value and not log data if false is
   * returned.
   *
   * @param typeString type string
   * @return False if type mismatch.
   */
  public boolean setType(String typeString) {
    synchronized (this) {
      if (m_type != null) {
        if (m_type.equals(typeString)) {
          return true;
        }
        typeMismatch(typeString);
        return false;
      }
      m_type = typeString;
    }
    log(".type", typeString);
    return true;
  }

  private void typeMismatch(String typeString) {
    TelemetryRegistry.reportWarning(
        m_path, "table type mismatch, expected '" + getType() + "', got '" + typeString + "'");
  }

  /**
   * Gets the table type.
   *
   * @return Table type as set by setType(), or null if none set.
   */
  public String getType() {
    synchronized (this) {
      return m_type;
    }
  }

  /**
   * Gets a child telemetry table.
   *
   * @param name table name
   * @return table
   */
  public TelemetryTable getTable(String name) {
    return m_tablesMap.computeIfAbsent(name, k -> TelemetryRegistry.getTable(m_path + k + "/"));
  }

  /**
   * Gets a telemetry entry.
   *
   * @param name name
   * @return entry
   */
  private TelemetryEntry getEntry(String name) {
    TelemetryEntry entry = m_entriesMap.get(name);
    if (entry != null) {
      return entry;
    }
    return m_entriesMap.computeIfAbsent(name, k -> TelemetryRegistry.getEntry(m_path + k));
  }

  /**
   * Indicates duplicate values should be preserved. Normally duplicate values are ignored.
   *
   * @param name the name
   */
  public void keepDuplicates(String name) {
    getEntry(name).keepDuplicates();
  }

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param name the name
   * @param key property key
   * @param value property value
   */
  public void setProperty(String name, String key, String value) {
    getEntry(name).setProperty(key, value);
  }

  /**
   * Logs a generic object.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   */
  public <T> void log(String name, T value) {
    switch (value) {
      case TelemetryLoggable v -> {
        TelemetryTable table = getTable(name);
        String typeString = v.getTelemetryType();
        boolean setType = false;
        if (typeString != null) {
          synchronized (table) {
            if (table.m_type == null) {
              setType = true;
            } else if (!table.m_type.equals(typeString)) {
              table.typeMismatch(typeString);
              return;
            }
          }
        }
        v.logTo(table);
        if (setType) {
          table.setType(typeString);
        }
      }
      case StructSerializable v -> {
        var lookup = s_structLookupCache.get(v.getClass());
        if (lookup.warning() != null) {
          TelemetryRegistry.reportWarning(m_path + name, lookup.warning());
          return;
        }
        switch (lookup.value()) {
          case Struct<?> s when s.getTypeClass().equals(value.getClass()) -> {
            @SuppressWarnings("unchecked")
            Struct<T> s2 = (Struct<T>) s;
            log(name, value, s2);
          }
          case Struct<?> s ->
              TelemetryRegistry.reportWarning(
                  m_path + name,
                  "type mismatch, expected '"
                      + s.getTypeClass().getName()
                      + "', got '"
                      + value.getClass().getName()
                      + "'");
          default ->
              TelemetryRegistry.reportWarning(
                  m_path + name,
                  "struct field for " + v.getClass().getName() + " is not of Struct<?> type");
        }
      }
      case ProtobufSerializable v -> {
        var lookup = s_protoLookupCache.get(v.getClass());
        if (lookup.warning() != null) {
          TelemetryRegistry.reportWarning(m_path + name, lookup.warning());
          return;
        }
        switch (lookup.value()) {
          case Protobuf<?, ?> s when s.getTypeClass().equals(value.getClass()) -> {
            @SuppressWarnings("unchecked")
            Protobuf<T, ?> s2 = (Protobuf<T, ?>) s;
            log(name, value, s2);
          }
          case Protobuf<?, ?> s ->
              TelemetryRegistry.reportWarning(
                  m_path + name,
                  "type mismatch, expected '"
                      + s.getTypeClass().getName()
                      + "', got '"
                      + value.getClass().getName()
                      + "'");
          default ->
              TelemetryRegistry.reportWarning(
                  m_path + name,
                  "proto field for " + v.getClass().getName() + " is not of Protobuf<?, ?> type");
        }
      }
      case Boolean v -> log(name, v.booleanValue());
      case Float v -> log(name, v.floatValue());
      case Double v -> log(name, v.doubleValue());
      case Number v -> log(name, v.longValue());
      case String v -> log(name, v);
      default -> {
        // try other handlers
        var handler = TelemetryRegistry.getTypeHandler(value);
        if (handler != null) {
          handler.logTo(this, name, value);
        } else {
          // fall back to string
          log(name, value.toString());
        }
      }
    }
  }

  /**
   * Logs an object with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer
   */
  public <T> void log(String name, T value, Struct<T> struct) {
    getEntry(name).logStruct(value, struct);
  }

  /**
   * Logs an object with a Protobuf serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param proto protobuf serializer
   */
  public <T> void log(String name, T value, Protobuf<T, ?> proto) {
    getEntry(name).logProtobuf(value, proto);
  }

  /**
   * Logs a generic array.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   */
  public <T> void log(String name, T[] value) {
    switch (value) {
      case StructSerializable[] v -> {
        Class<?> componentType = value.getClass().getComponentType();
        var lookup = s_structLookupCache.get(componentType);
        if (lookup.warning() != null) {
          TelemetryRegistry.reportWarning(m_path + name, lookup.warning());
          return;
        }
        switch (lookup.value()) {
          case Struct<?> s when s.getTypeClass().equals(componentType) -> {
            @SuppressWarnings("unchecked")
            Struct<T> s2 = (Struct<T>) s;
            log(name, value, s2);
          }
          case Struct<?> s ->
              TelemetryRegistry.reportWarning(
                  m_path + name,
                  "type mismatch, expected '"
                      + s.getTypeClass().getName()
                      + "', got '"
                      + value.getClass().getName()
                      + "'");
          default ->
              TelemetryRegistry.reportWarning(
                  m_path + name,
                  "struct field for " + componentType.getName() + " is not of Struct<?> type");
        }
      }
      case Boolean[] v -> {
        boolean[] arr = new boolean[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].booleanValue();
        }
        log(name, arr);
      }
      case Float[] v -> {
        float[] arr = new float[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].floatValue();
        }
        log(name, arr);
      }
      case Double[] v -> {
        double[] arr = new double[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].doubleValue();
        }
        log(name, arr);
      }
      case Number[] v -> {
        long[] arr = new long[v.length];
        for (int i = 0; i < v.length; i++) {
          arr[i] = v[i].longValue();
        }
        log(name, arr);
      }
      default -> {
        // TODO: use other Object handler?
        // fall back to string array
        String[] strs = new String[value.length];
        for (int i = 0; i < value.length; i++) {
          strs[i] = value[i].toString();
        }
        log(name, strs);
      }
    }
  }

  /**
   * Logs an array of objects with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer
   */
  public <T> void log(String name, T[] value, Struct<T> struct) {
    getEntry(name).logStructArray(value, struct);
  }

  /**
   * Logs a boolean.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, boolean value) {
    getEntry(name).logBoolean(value);
  }

  /**
   * Logs a byte.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, byte value) {
    getEntry(name).logByte(value);
  }

  /**
   * Logs a short.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, short value) {
    getEntry(name).logShort(value);
  }

  /**
   * Logs an int.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, int value) {
    getEntry(name).logInt(value);
  }

  /**
   * Logs a long.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, long value) {
    getEntry(name).logLong(value);
  }

  /**
   * Logs a float.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, float value) {
    getEntry(name).logFloat(value);
  }

  /**
   * Logs a double.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, double value) {
    getEntry(name).logDouble(value);
  }

  /**
   * Logs a String.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, String value) {
    getEntry(name).logString(value, "string");
  }

  /**
   * Logs a String with a custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  public void log(String name, String value, String typeString) {
    getEntry(name).logString(value, typeString);
  }

  /**
   * Logs a boolean array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, boolean[] value) {
    getEntry(name).logBooleanArray(value);
  }

  /**
   * Logs a short array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, short[] value) {
    getEntry(name).logShortArray(value);
  }

  /**
   * Logs an int array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, int[] value) {
    getEntry(name).logIntArray(value);
  }

  /**
   * Logs a long array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, long[] value) {
    getEntry(name).logLongArray(value);
  }

  /**
   * Logs a float array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, float[] value) {
    getEntry(name).logFloatArray(value);
  }

  /**
   * Logs a double array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, double[] value) {
    getEntry(name).logDoubleArray(value);
  }

  /**
   * Logs a String array.
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, String[] value) {
    getEntry(name).logStringArray(value);
  }

  /**
   * Logs a raw value (byte array).
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, byte[] value) {
    getEntry(name).logRaw(value, "raw");
  }

  /**
   * Logs a raw value (byte array) with a custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  public void log(String name, byte[] value, String typeString) {
    getEntry(name).logRaw(value, typeString);
  }
}
