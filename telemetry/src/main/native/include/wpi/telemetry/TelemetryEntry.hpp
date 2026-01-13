// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>

namespace wpi {

/**
 * Interface for individual telemetry entries. Intended for use by backends;
 * most users should use the TelemetryTable class.
 */
class TelemetryEntry {
 public:
  TelemetryEntry() = default;
  virtual ~TelemetryEntry() = default;
  TelemetryEntry(const TelemetryEntry&) = delete;
  TelemetryEntry& operator=(const TelemetryEntry&) = delete;

  /**
   * Indicates duplicate values should be preserved. Normally duplicate values
   * are ignored.
   */
  virtual void KeepDuplicates() = 0;

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param key property key
   * @param value property value
   */
  virtual void SetProperty(std::string_view key, std::string_view value) = 0;

  /**
   * Logs a boolean.
   *
   * @param value the value
   */
  virtual void LogBoolean(bool value) = 0;

  /**
   * Logs a byte.
   *
   * @param value the value
   */
  virtual void LogInt8(int8_t value) { LogInt64(value); }

  /**
   * Logs a short.
   *
   * @param value the value
   */
  virtual void LogInt16(int16_t value) { LogInt64(value); }

  /**
   * Logs an int.
   *
   * @param value the value
   */
  virtual void LogInt32(int32_t value) { LogInt64(value); }

  /**
   * Logs a long.
   *
   * @param value the value
   */
  virtual void LogInt64(int64_t value) = 0;

  /**
   * Logs a float.
   *
   * @param value the value
   */
  virtual void LogFloat(float value) = 0;

  /**
   * Logs a double.
   *
   * @param value the value
   */
  virtual void LogDouble(double value) = 0;

  /**
   * Logs a String.
   *
   * @param value the value
   * @param typeString the type string
   */
  virtual void LogString(std::string_view value,
                         std::string_view typeString) = 0;

  /**
   * Logs a boolean array.
   *
   * @param value the value
   */
  virtual void LogBooleanArray(std::span<const bool> value) = 0;

  /**
   * Logs a boolean array.
   *
   * @param value the value
   */
  virtual void LogBooleanArray(std::span<const int> value) = 0;

  /**
   * Logs a short array.
   *
   * @param value the value
   */
  virtual void LogInt16Array(std::span<const int16_t> value) = 0;

  /**
   * Logs an int array.
   *
   * @param value the value
   */
  virtual void LogInt32Array(std::span<const int32_t> value) = 0;

  /**
   * Logs a long array.
   *
   * @param value the value
   */
  virtual void LogInt64Array(std::span<const int64_t> value) = 0;

  /**
   * Logs a float array.
   *
   * @param value the value
   */
  virtual void LogFloatArray(std::span<const float> value) = 0;

  /**
   * Logs a double array.
   *
   * @param value the value
   */
  virtual void LogDoubleArray(std::span<const double> value) = 0;

  /**
   * Logs a String array.
   *
   * @param value the value
   */
  virtual void LogStringArray(std::span<const std::string> value) = 0;

  /**
   * Logs a String array.
   *
   * @param value the value
   */
  virtual void LogStringArray(std::span<const std::string_view> value) = 0;

  /**
   * Logs a raw value (byte array).
   *
   * @param value the value
   * @param typeString the type string
   */
  virtual void LogRaw(std::span<const uint8_t> value,
                      std::string_view typeString) = 0;
};

}  // namespace wpi
