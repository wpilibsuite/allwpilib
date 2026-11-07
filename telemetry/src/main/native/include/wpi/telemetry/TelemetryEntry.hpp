// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>

namespace wpi::telemetry {

/**
 * Interface for individual telemetry entries. Intended for use by backends;
 * most users should use the TelemetryTable class.
 *
 * Implementations must not throw from logging, metadata, or discard-checking
 * methods. Recoverable failures should be reported through
 * TelemetryRegistry::ReportWarning() and skipped.
 *
 * Methods on the same entry may be called concurrently. Implementations must
 * copy spans and string views they retain after a method returns.
 */
class TelemetryEntry {
 public:
  TelemetryEntry() = default;
  virtual ~TelemetryEntry() = default;
  TelemetryEntry(const TelemetryEntry&) = delete;
  TelemetryEntry& operator=(const TelemetryEntry&) = delete;

  /**
   * Checks if this entry discards data.
   *
   * @return True if this entry discards logged data
   */
  virtual bool IsDiscard() const { return false; }

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
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogBoolean(bool value, int64_t timestamp) = 0;

  /**
   * Logs a byte.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogInt8(int8_t value, int64_t timestamp) {
    LogInt64(value, timestamp);
  }

  /**
   * Logs a short.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogInt16(int16_t value, int64_t timestamp) {
    LogInt64(value, timestamp);
  }

  /**
   * Logs an int.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogInt32(int32_t value, int64_t timestamp) {
    LogInt64(value, timestamp);
  }

  /**
   * Logs a long.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogInt64(int64_t value, int64_t timestamp) = 0;

  /**
   * Logs a float.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogFloat(float value, int64_t timestamp) = 0;

  /**
   * Logs a double.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogDouble(double value, int64_t timestamp) = 0;

  /**
   * Logs a String.
   *
   * @param value the value
   * @param typeString the type string
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogString(std::string_view value, std::string_view typeString,
                         int64_t timestamp) = 0;

  /**
   * Logs a boolean array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogBooleanArray(std::span<const bool> value,
                               int64_t timestamp) = 0;

  /**
   * Logs a boolean array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogBooleanArray(std::span<const int> value,
                               int64_t timestamp) = 0;

  /**
   * Logs a short array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogInt16Array(std::span<const int16_t> value,
                             int64_t timestamp) = 0;

  /**
   * Logs an int array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogInt32Array(std::span<const int32_t> value,
                             int64_t timestamp) = 0;

  /**
   * Logs a long array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogInt64Array(std::span<const int64_t> value,
                             int64_t timestamp) = 0;

  /**
   * Logs a float array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogFloatArray(std::span<const float> value,
                             int64_t timestamp) = 0;

  /**
   * Logs a double array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogDoubleArray(std::span<const double> value,
                              int64_t timestamp) = 0;

  /**
   * Logs a String array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogStringArray(std::span<const std::string> value,
                              int64_t timestamp) = 0;

  /**
   * Logs a String array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogStringArray(std::span<const std::string_view> value,
                              int64_t timestamp) = 0;

  /**
   * Logs a raw value (byte array).
   *
   * @param value the value
   * @param typeString the type string
   * @param timestamp timestamp in nanoseconds in the same time base as
   *     wpi::util::Now(), or 0 to use the current time
   */
  virtual void LogRaw(std::span<const uint8_t> value,
                      std::string_view typeString, int64_t timestamp) = 0;
};

}  // namespace wpi::telemetry
