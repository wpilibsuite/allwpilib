// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include "wpi/telemetry/TelemetryTable.hpp"

namespace wpi {

class Telemetry final {
 public:
  Telemetry() = delete;

  /**
   * Gets the root telemetry table.
   *
   * @return table
   */
  static wpi::TelemetryTable& GetTable();

  /**
   * Gets a child telemetry table.
   *
   * @param name table name
   * @return table
   */
  static wpi::TelemetryTable& GetTable(std::string_view name) {
    return GetTable().GetTable(name);
  }

  /**
   * Indicates duplicate values should be preserved. Normally duplicate values
   * are ignored.
   *
   * @param name the name
   */
  static void KeepDuplicates(std::string_view name) {
    GetTable().KeepDuplicates(name);
  }

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param name the name
   * @param key property key
   * @param value property value
   */
  static void SetProperty(std::string_view name, std::string_view key,
                          std::string_view value) {
    GetTable().SetProperty(name, key, value);
  }

  /**
   * Logs an object.
   *
   * @param name the name
   * @param value the value
   * @param info type parameters for struct serializer (optional)
   */
  template <typename T, typename... I>
  static void Log(std::string_view name, const T& value, I... info) {
    GetTable().Log(name, value, info...);
  }

  /**
   * Logs an array of objects with a Struct serializer.
   *
   * @param name the name
   * @param value the value
   * @param info type parameters for struct serializer (optional)
   */
  template <typename T, typename... I>
  static void Log(std::string_view name, std::span<const T> value, I... info) {
    GetTable().Log(name, value, info...);
  }

  /**
   * Logs a boolean.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, bool value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a byte.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, int8_t value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a short.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, int16_t value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs an int.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, int32_t value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a long.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, int64_t value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a float.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, float value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a double.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, double value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a String.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::string_view value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a String with custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  static void Log(std::string_view name, std::string_view value,
                  std::string_view typeString) {
    GetTable().Log(name, value, typeString);
  }

  /**
   * Logs a boolean array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const bool> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a short array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const int16_t> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs an int array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const int32_t> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a long array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const int64_t> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a float array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const float> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a double array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const double> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a String array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const std::string> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a String array.
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name,
                  std::span<const std::string_view> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a raw value (byte array).
   *
   * @param name the name
   * @param value the value
   */
  static void Log(std::string_view name, std::span<const uint8_t> value) {
    GetTable().Log(name, value);
  }

  /**
   * Logs a raw value (byte array) with custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  static void Log(std::string_view name, std::span<const uint8_t> value,
                  std::string_view typeString) {
    GetTable().Log(name, value, typeString);
  }
};

}  // namespace wpi
