// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include "wpi/StringMap.h"
#include "wpi/mutex.h"

namespace wpi {

class TelemetryEntry;
class TelemetryRegistry;

/**
 * Telemetry sends information from the robot program to dashboards, debug
 * tools, or log files.
 *
 * For more advanced use cases, use the NetworkTables or DataLog APIs.
 */
class TelemetryTable final {
  friend class TelemetryRegistry;
  struct private_init {};

 public:
  /**
   * Constructs a telemetry table. Only usable internally.
   *
   * @param path path with trailing "/".
   */
  explicit TelemetryTable(std::string_view path, const private_init&)
      : m_path{path} {}

  TelemetryTable(const TelemetryTable&) = delete;
  TelemetryTable& operator=(const TelemetryTable&) = delete;

  /**
   * Gets the table path.
   *
   * @return path with trailing "/"
   */
  std::string_view GetPath() const;

  /**
   * Gets a child telemetry table.
   *
   * @param name table name
   * @return table
   */
  TelemetryTable& GetTable(std::string_view name) const;

  /**
   * Indicates duplicate values should be preserved. Normally duplicate values
   * are ignored.
   *
   * @param name the name
   */
  void KeepDuplicates(std::string_view name);

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param name the name
   * @param key property key
   * @param value property value
   */
  void SetProperty(std::string_view name, std::string_view key,
                   std::string_view value);

  /**
   * Sets custom data type for a value. Generally not necessary.
   *
   * @param name the name
   * @param typeString type string
   */
  void SetTypeString(std::string_view name, std::string_view typeString);
#if 0
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
#endif
  template <typename T>
  void Log(std::string_view name, T value) {
    if constexpr (std::is_integral_v<T>) {
      Log(name, static_cast<int64_t>(value));
    } else if constexpr (std::is_floating_point_v<T>) {
      Log(name, static_cast<double>(value));
    }
  }

  /**
   * Logs a boolean.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, bool value);

  /**
   * Logs a byte.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, int8_t value);

  /**
   * Logs a short.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, int16_t value);

  /**
   * Logs an int.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, int32_t value);

  /**
   * Logs a long.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, int64_t value);

  /**
   * Logs a float.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, float value);

  /**
   * Logs a double.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, double value);

  /**
   * Logs a String.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::string_view value);

  /**
   * Logs a boolean array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const bool> value);

  /**
   * Logs a byte array (raw value).
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const uint8_t> value);

  /**
   * Logs a short array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const int16_t> value);

  /**
   * Logs an int array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const int32_t> value);

  /**
   * Logs a long array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const int64_t> value);

  /**
   * Logs a float array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const float> value);

  /**
   * Logs a double array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const double> value);

  /**
   * Logs a String array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const std::string> value);

  /**
   * Logs a String array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const std::string_view> value);

 private:
  /**
   * Gets a telemetry entry.
   *
   * @param name name
   * @return entry
   */
  TelemetryEntry& GetEntry(std::string_view name);

  /** Clears the table's cached entries. */
  void Reset();

  mutable wpi::mutex m_mutex;
  std::string m_path;
  mutable wpi::StringMap<TelemetryTable*> m_tablesMap;
  mutable wpi::StringMap<TelemetryEntry*> m_entriesMap;
};

}  // namespace wpi
