// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.protobuf.ProtobufSerializable;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructSerializable;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

/**
 * Telemetry sends information from the robot program to dashboards, debug tools, or log files.
 *
 * <p>For more advanced use cases, use the NetworkTables or DataLog APIs.
 */
public class TelemetryTable {
  private final String m_path;
  private final ConcurrentMap<String, TelemetryTable> m_tablesMap = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, TelemetryEntry> m_entriesMap = new ConcurrentHashMap<>();

  /**
   * Constructs a telemetry table.
   *
   * @param path path with trailing "/".
   */
  public TelemetryTable(String path) {
    m_path = path;
    TelemetryRegistry.addTable(this);
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
   * Gets a child telemetry table.
   *
   * @param name table name
   * @return table
   */
  public TelemetryTable getTable(String name) {
    return m_tablesMap.computeIfAbsent(name, k -> new TelemetryTable(m_path + k + "/"));
  }

  /**
   * Gets a telemetry entry.
   *
   * @param name name
   * @return entry
   */
  protected TelemetryEntry getEntry(String name) {
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
   * Sets custom data type for a value. Generally not necessary.
   *
   * @param name the name
   * @param typeString type string
   */
  public void setTypeString(String name, String typeString) {
    getEntry(name).setTypeString(typeString);
  }

  /**
   * Logs a generic object.
   *
   * @param name the name
   * @param value the value
   */
  public <T> void log(String name, T value) {
    if (value instanceof TelemetryLoggable) {
      ((TelemetryLoggable) value).log(getTable(name));
    } else if (value instanceof StructSerializable v) {
      // use introspection to get "struct" static variable
      Object obj;
      try {
        obj = v.getClass().getField("struct").get(null);
      } catch (NoSuchFieldException e) {
        // TODO: warn
        return;
      } catch (IllegalAccessException e) {
        // TODO: warn
        return;
      }
      if (obj instanceof Struct<?> s && s.getTypeClass().equals(value.getClass())) {
        @SuppressWarnings("unchecked")
        Struct<T> s2 = (Struct<T>) s;
        log(name, value, s2);
      } else {
        // TODO: warn
      }
    } else if (value instanceof ProtobufSerializable v) {
      // use introspection to get "proto" static variable
      Object obj;
      try {
        obj = v.getClass().getField("proto").get(null);
      } catch (NoSuchFieldException e) {
        // TODO: warn
        return;
      } catch (IllegalAccessException e) {
        // TODO: warn
        return;
      }
      if (obj instanceof Protobuf<?, ?> s && s.getTypeClass().equals(value.getClass())) {
        @SuppressWarnings("unchecked")
        Protobuf<T, ?> s2 = (Protobuf<T, ?>) s;
        log(name, value, s2);
      } else {
        // TODO: warn
      }
    } else if (value instanceof Boolean v) {
      log(name, v.booleanValue());
    } else if (value instanceof Float v) {
      log(name, v.floatValue());
    } else if (value instanceof Double v) {
      log(name, v.doubleValue());
    } else if (value instanceof Number v) {
      log(name, v.longValue());
    } else if (value instanceof String v) {
      log(name, v);
    } else {
      // try other handlers
      var handler = TelemetryRegistry.getTypeHandler(value);
      if (handler.entryHandler() != null) {
        handler.entryHandler().accept(value, getEntry(name));
      } else if (handler.tableHandler() != null) {
        handler.tableHandler().accept(value, getTable(name));
      } else {
        // fall back to string
        log(name, value.toString());
      }
    }
  }

  /**
   * Logs an object with a Struct serializer.
   *
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
   * @param name the name
   * @param value the value
   */
  public <T> void log(String name, T[] value) {
    if (value instanceof StructSerializable[] v) {
      // use introspection to get "struct" static variable
      Object obj;
      try {
        obj = v.getClass().getField("struct").get(null);
      } catch (NoSuchFieldException e) {
        // TODO: warn
        return;
      } catch (IllegalAccessException e) {
        // TODO: warn
        return;
      }
      if (obj instanceof Struct<?> s
          && s.getTypeClass().equals(value.getClass().getComponentType())) {
        @SuppressWarnings("unchecked")
        Struct<T> s2 = (Struct<T>) s;
        log(name, value, s2);
      } else {
        // TODO: warn
      }
      /* TODO:
      } else if (value instanceof Boolean[] v) {
        log(name, v.booleanValue());
      } else if (value instanceof Float[] v) {
        log(name, v.floatValue());
      } else if (value instanceof Double[] v) {
        log(name, v.doubleValue());
      } else if (value instanceof Number[] v) {
        log(name, v.longValue());
      */
    } else {
      // TODO: use other Object handler?
      // fall back to string array
      String[] strs = new String[value.length];
      for (int i = 0; i < value.length; i++) {
        strs[i] = value[i].toString();
      }
      log(name, strs);
    }
  }

  /**
   * Logs an array of objects with a Struct serializer.
   *
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
    getEntry(name).logString(value);
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
   * Logs a byte array (raw value).
   *
   * @param name the name
   * @param value the value
   */
  public void log(String name, byte[] value) {
    getEntry(name).logByteArray(value);
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
}
