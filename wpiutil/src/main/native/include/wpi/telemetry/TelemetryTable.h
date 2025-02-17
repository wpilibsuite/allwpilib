// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include "wpi/SmallVector.h"
#include "wpi/StringMap.h"
#include "wpi/mutex.h"
#include "wpi/protobuf/Protobuf.h"
#include "wpi/struct/Struct.h"
#include "wpi/telemetry/TelemetryRegistry.h"

namespace wpi {

class TelemetryEntry;
class TelemetryTable;

namespace impl {
template <typename T, typename... I>
concept IsTelemetryLoggableADL =
    requires(TelemetryTable& table, const T& value, I... info) {
      Log(table, value, info...);
    };

template <typename T, typename... I>
inline void TelemetryLogADL(TelemetryTable& table, const T& value, I... info) {
  Log(table, value, info...);
}
}  // namespace impl

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

  /**
   * Logs an object.
   *
   * @param name the name
   * @param value the value
   * @param info type parameters for struct serializer (optional)
   */
  template <typename T, typename... I>
  void Log(std::string_view name, const T& value, I... info) {
    if constexpr (std::is_integral_v<T>) {
      Log(name, static_cast<int64_t>(value));
    } else if constexpr (std::is_floating_point_v<T>) {
      Log(name, static_cast<double>(value));
    } else if constexpr (impl::IsTelemetryLoggableADL<T, I...>) {
      impl::TelemetryLogADL(GetTable(name), value, info...);
    } else if constexpr (wpi::StructSerializable<T, I...>) {
      using S = wpi::Struct<T, I...>;
      TelemetryRegistry::AddStructSchema<T>(info...);
      if constexpr (sizeof...(I) == 0) {
        if constexpr (wpi::is_constexpr([] { S::GetSize(); })) {
          uint8_t buf[S::GetSize()];
          S::Pack(buf, value);
          LogRaw(name, S::GetTypeString(), buf);
          return;
        }
      }
      wpi::SmallVector<uint8_t, 128> buf;
      buf.resize_for_overwrite(S::GetSize(info...));
      S::Pack(buf, value, info...);
      LogRaw(name, S::GetTypeString(info...), buf);
    } else if constexpr (wpi::ProtobufSerializable<T>) {
      wpi::ProtobufMessage<T> msg;
      TelemetryRegistry::AddProtobufSchema<T>(msg);
      wpi::SmallVector<uint8_t, 128> buf;
      msg.Pack(buf, value);
      LogRaw(name, msg.GetTypeString(), buf);
    } else {
      // TODO: see if it's convertable with fmt::to_string?
      // TODO: any option for type handlers ala Java?
      static_assert(false, "Don't know how to serialize type");
    }
  }

  /**
   * Logs an array of objects with a Struct serializer.
   *
   * @param name the name
   * @param value the value
   * @param struct struct serializer
   */
  template <typename T, typename... I>
  void Log(std::string_view name, std::span<const T> value, I... info) {
    if constexpr (wpi::StructSerializable<T, I...>) {
      TelemetryRegistry::AddStructSchema<T>(info...);
      using S = wpi::Struct<T, I...>;
      wpi::StructArrayBuffer<T, I...> buf;
      buf.Write(
          value,
          [&](auto bytes) { LogRaw(name, S::GetTypeString(info...), bytes); },
          info...);
    } else {
      // TODO: see if it's convertable with fmt::to_string?
      static_assert(false, "Don't know how to serialize type");
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

  /**
   * Logs a byte array (raw value) with type string matching.
   *
   * @param name the name
   * @param typeString the type string
   * @param value the value
   */
  void LogRaw(std::string_view name, std::string_view typeString,
              std::span<const uint8_t> value);

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
