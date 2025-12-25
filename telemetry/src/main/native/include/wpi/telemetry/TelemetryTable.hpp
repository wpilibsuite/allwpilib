// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include <fmt/format.h>

#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace wpi {

class TelemetryEntry;
class TelemetryTable;

namespace impl {
template <typename T>
struct always_false : std::false_type {};

template <typename T>
concept IsSpan =
    std::same_as<std::span<const typename T::value_type, T::extent>, T> ||
    std::same_as<std::span<typename T::value_type, T::extent>, T>;

template <typename T>
concept IsSpanConvertible =
    std::constructible_from<std::span<const typename T::value_type>, T> ||
    std::is_bounded_array_v<T>;
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
   * Sets the table type.  TelemetryLoggable implementations can use this
   * function to communicate the type of data in the table.  Callers should
   * check the return value and not log data if false is returned.
   *
   * @param typeString type string
   * @return False if type mismatch.
   */
  bool SetType(std::string_view typeString);

  /**
   * Gets the table type.
   *
   * @return Table type as set by SetType(), or empty string if none set.
   */
  std::string GetType() const;

  /**
   * Returns if a table type is set.
   *
   * @return True if type set by SetType(), false otherwise
   */
  bool HasType() const;

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
   * Logs an object.
   *
   * @param name the name
   * @param value the value
   * @param info type parameters for struct serializer (optional)
   */
  template <typename T, typename... I>
    requires(!impl::IsSpan<T>)
  void Log(std::string_view name, const T& value, I... info) {
    if constexpr (SupportsTelemetryWithTypeName<T, I...>) {
      auto& table = GetTable(name);
      auto typeString = GetTelemetryTypeName(value, info...);
      bool setType = false;
      if (!std::string_view{typeString}.empty()) {
        std::scoped_lock lock{m_mutex};
        if (table.m_type.empty()) {
          setType = true;
        } else if (table.m_type != typeString) {
          table.TypeMismatch(typeString);
          return;
        }
      }
      LogTo(table, value, info...);
      if (setType) {
        table.SetType(typeString);
      }
    } else if constexpr (SupportsTelemetry<T, I...>) {
      auto& table = GetTable(name);
      LogTo(table, value, info...);
    } else if constexpr (SupportsTelemetryValue<T, I...>) {
      LogValueTo(*this, name, value, info...);
    } else if constexpr (wpi::util::StructSerializable<T, I...>) {
      using S = wpi::util::Struct<T, I...>;
      auto backend = TelemetryRegistry::GetBackend(name);
      TelemetryRegistry::AddStructSchema<T>(*backend, info...);
      if constexpr (sizeof...(I) == 0) {
        if constexpr (wpi::util::is_constexpr([] { S::GetSize(); })) {
          uint8_t buf[S::GetSize()];
          S::Pack(buf, value);
          Log(name, std::span{buf}, std::string_view{S::GetTypeName()});
          return;
        }
      }
      wpi::util::SmallVector<uint8_t, 128> buf;
      buf.resize_for_overwrite(S::GetSize(info...));
      S::Pack(buf, value, info...);
      Log(name, std::span{buf}, std::string_view{S::GetTypeName(info...)});
    } else if constexpr (wpi::util::ProtobufSerializable<T>) {
      auto backend = TelemetryRegistry::GetBackend(name);
      wpi::util::ProtobufMessage<T> msg;
      TelemetryRegistry::AddProtobufSchema<T>(*backend, msg);
      wpi::util::SmallVector<uint8_t, 128> buf;
      msg.Pack(buf, value);
      Log(name, buf, msg.GetTypeString());
    } else if constexpr (std::integral<T>) {
      Log(name, static_cast<int64_t>(value));
    } else if constexpr (std::floating_point<T>) {
      Log(name, static_cast<double>(value));
    } else if constexpr (std::constructible_from<std::string_view, T>) {
      Log(name, std::string_view{value}, info...);
    } else if constexpr (std::constructible_from<fmt::formatter<T>>) {
      Log(name, std::string_view{fmt::to_string(value)});
    } else if constexpr (impl::IsSpanConvertible<T>) {
      Log(name, std::span{value}, info...);
    } else {
      static_assert(impl::always_false<T>::value,
                    "Don't know how to serialize type");
    }
  }

  /**
   * Logs an array of objects with a Struct serializer.
   *
   * @param name the name
   * @param value the value
   * @param info type parameters for struct serializer (optional)
   */
  template <typename T, typename... I>
  void Log(std::string_view name, std::span<const T> value, I... info) {
    if constexpr (wpi::util::StructSerializable<T, I...>) {
      auto backend = TelemetryRegistry::GetBackend(name);
      TelemetryRegistry::AddStructSchema<T>(*backend, info...);
      using S = wpi::util::Struct<T, I...>;
      wpi::util::StructArrayBuffer<T, I...> buf;
      buf.Write(
          value,
          [&](auto bytes) {
            Log(name, bytes, std::string_view{S::GetTypeName(info...)});
          },
          info...);
    } else if constexpr (std::constructible_from<fmt::formatter<T>>) {
      std::vector<std::string> strings;
      strings.reserve(value.size());
      for (auto&& v : value) {
        strings.emplace_back(fmt::to_string(value));
      }
      Log(name, strings);
    } else {
      static_assert(impl::always_false<T>::value,
                    "Don't know how to serialize type");
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
   * Logs a String with a custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  void Log(std::string_view name, std::string_view value,
           std::string_view typeString);

  /**
   * Logs a boolean array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const bool> value);

  /**
   * Logs a boolean array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::initializer_list<bool> value) {
    Log(name, std::span{value.begin(), value.end()});
  }

  /**
   * Logs a short array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const int16_t> value);

  /**
   * Logs a short array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::initializer_list<int16_t> value) {
    Log(name, std::span{value.begin(), value.end()});
  }

  /**
   * Logs an int array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const int32_t> value);

  /**
   * Logs a int array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::initializer_list<int32_t> value) {
    Log(name, std::span{value.begin(), value.end()});
  }

  /**
   * Logs a long array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const int64_t> value);

  /**
   * Logs a long array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::initializer_list<int64_t> value) {
    Log(name, std::span{value.begin(), value.end()});
  }

  /**
   * Logs a float array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const float> value);

  /**
   * Logs a float array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::initializer_list<float> value) {
    Log(name, std::span{value.begin(), value.end()});
  }

  /**
   * Logs a double array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const double> value);

  /**
   * Logs a double array.
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::initializer_list<double> value) {
    Log(name, std::span{value.begin(), value.end()});
  }

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
   * Logs a raw value (byte array).
   *
   * @param name the name
   * @param value the value
   */
  void Log(std::string_view name, std::span<const uint8_t> value);

  /**
   * Logs a raw value (byte array) with custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  void Log(std::string_view name, std::span<const uint8_t> value,
           std::string_view typeString);

 private:
  /**
   * Gets a telemetry entry.
   *
   * @param name name
   * @return entry
   */
  TelemetryEntry& GetEntry(std::string_view name);

  void TypeMismatch(std::string_view typeString);

  /** Clears the table's cached entries. */
  void Reset();

  mutable wpi::util::mutex m_mutex;
  std::string m_path;
  mutable wpi::util::StringMap<TelemetryTable*> m_tablesMap;
  mutable wpi::util::StringMap<TelemetryEntry*> m_entriesMap;
  std::string m_type;
};

}  // namespace wpi
