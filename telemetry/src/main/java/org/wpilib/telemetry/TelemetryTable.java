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
  private final String m_path;
  private final ConcurrentMap<String, TelemetryTable> m_tablesMap = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, TelemetryEntry> m_entriesMap = new ConcurrentHashMap<>();
  private String m_type;

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
    if (value instanceof TelemetryLoggable v) {
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
      v.updateTelemetry(table);
      if (setType) {
        table.setType(typeString);
      }
    } else if (value instanceof StructSerializable v) {
      // use introspection to get "struct" static variable
      Object obj;
      try {
        obj = v.getClass().getField("struct").get(null);
      } catch (NoSuchFieldException e) {
        TelemetryRegistry.reportWarning(
            m_path + name, "could not get struct field for " + v.getClass().getName());
        return;
      } catch (IllegalAccessException e) {
        TelemetryRegistry.reportWarning(
            m_path + name, "could not access struct field for " + v.getClass().getName());
        return;
      }
      if (obj instanceof Struct<?> s) {
        if (s.getTypeClass().equals(value.getClass())) {
          @SuppressWarnings("unchecked")
          Struct<T> s2 = (Struct<T>) s;
          log(name, value, s2);
        } else {
          TelemetryRegistry.reportWarning(
              m_path + name,
              "type mismatch, expected '"
                  + s.getTypeClass().getName()
                  + "', got '"
                  + value.getClass().getName()
                  + "'");
        }
      } else {
        TelemetryRegistry.reportWarning(
            m_path + name,
            "struct field for " + v.getClass().getName() + " is not of Struct<?> type");
      }
    } else if (value instanceof ProtobufSerializable v) {
      // use introspection to get "proto" static variable
      Object obj;
      try {
        obj = v.getClass().getField("proto").get(null);
      } catch (NoSuchFieldException e) {
        TelemetryRegistry.reportWarning(
            m_path + name, "could not get proto field for " + v.getClass().getName());
        return;
      } catch (IllegalAccessException e) {
        TelemetryRegistry.reportWarning(
            m_path + name, "could not access proto field for " + v.getClass().getName());
        return;
      }
      if (obj instanceof Protobuf<?, ?> s) {
        if (s.getTypeClass().equals(value.getClass())) {
          @SuppressWarnings("unchecked")
          Protobuf<T, ?> s2 = (Protobuf<T, ?>) s;
          log(name, value, s2);
        } else {
          TelemetryRegistry.reportWarning(
              m_path + name,
              "type mismatch, expected '"
                  + s.getTypeClass().getName()
                  + "', got '"
                  + value.getClass().getName()
                  + "'");
        }
      } else {
        TelemetryRegistry.reportWarning(
            m_path + name,
            "proto field for " + v.getClass().getName() + " is not of Protobuf<?, ?> type");
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
      if (handler != null) {
        handler.logTo(this, name, value);
      } else {
        // fall back to string
        log(name, value.toString());
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
    if (value instanceof StructSerializable[] v) {
      // use introspection to get "struct" static variable
      Object obj;
      try {
        obj = v.getClass().getField("struct").get(null);
      } catch (NoSuchFieldException e) {
        TelemetryRegistry.reportWarning(
            m_path + name, "could not get struct field for " + v.getClass().getName());
        return;
      } catch (IllegalAccessException e) {
        TelemetryRegistry.reportWarning(
            m_path + name, "could not access struct field for " + v.getClass().getName());
        return;
      }
      if (obj instanceof Struct<?> s) {
        if (s.getTypeClass().equals(value.getClass().getComponentType())) {
          @SuppressWarnings("unchecked")
          Struct<T> s2 = (Struct<T>) s;
          log(name, value, s2);
        } else {
          TelemetryRegistry.reportWarning(
              m_path + name,
              "type mismatch, expected '"
                  + s.getTypeClass().getName()
                  + "', got '"
                  + value.getClass().getName()
                  + "'");
        }
      } else {
        TelemetryRegistry.reportWarning(
            m_path + name,
            "struct field for " + v.getClass().getName() + " is not of Struct<?> type");
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
