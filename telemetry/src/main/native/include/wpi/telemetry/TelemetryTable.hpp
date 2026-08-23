// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <exception>
#include <format>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryLoggable.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace wpi::telemetry {

class TelemetryBackend;
class TelemetryEntry;
class TelemetryTable;

namespace python {
class PyTelemetryTable;
}  // namespace python

namespace impl {
template <typename T>
struct always_false : std::false_type {};

template <typename T>
concept IsSpan =
    std::same_as<std::span<const typename T::value_type, T::extent>, T> ||
    std::same_as<std::span<typename T::value_type, T::extent>, T>;

template <typename T>
concept HasValueType =
    requires { typename std::remove_cvref_t<T>::value_type; };

template <typename T>
concept IsSpanConvertible =
    (HasValueType<T> &&
     std::constructible_from<
         std::span<const typename std::remove_cvref_t<T>::value_type>,
         const T&>) ||
    std::is_bounded_array_v<std::remove_cvref_t<T>>;
}  // namespace impl

/**
 * Telemetry sends information from the robot program to dashboards, debug
 * tools, or log files.
 *
 * For more advanced use cases, use the NetworkTables or DataLog APIs.
 */
class TelemetryTable final {
  friend class TelemetryRegistry;
  friend class python::PyTelemetryTable;
  struct private_init {};

  /**
   * Handle for a telemetry entry. This keeps the entry's backend alive while
   * the entry is in use.
   */
  class EntryHandle {
    friend class TelemetryTable;

   private:
    struct CachedEntry {
      CachedEntry(std::string_view name, uint64_t resetGeneration,
                  std::shared_ptr<TelemetryEntry> entry,
                  std::shared_ptr<TelemetryBackend> backend)
          : name{name},
            resetGeneration{resetGeneration},
            entry{std::move(entry)},
            backend{std::move(backend)} {}

      std::string name;
      uint64_t resetGeneration;
      std::shared_ptr<TelemetryEntry> entry;
      std::shared_ptr<TelemetryBackend> backend;
      mutable wpi::util::mutex schemaMutex;
      wpi::util::StringMap<bool> publishedSchemas;
    };

   public:
    /** Checks if this handle is non-empty. */
    explicit operator bool() const { return static_cast<bool>(m_entry); }

    /** Gets the wrapped telemetry entry. */
    TelemetryEntry& operator*() const { return *m_entry->entry; }

    /** Gets the wrapped telemetry entry. */
    TelemetryEntry* operator->() const { return m_entry->entry.get(); }

    /** Gets the backend that owns the entry. */
    TelemetryBackend& GetBackend() const { return *m_entry->backend; }

    bool HasPublishedSchema(std::string_view schemaName) const {
      std::scoped_lock lock{m_entry->schemaMutex};
      return m_entry->publishedSchemas.contains(schemaName);
    }

    void MarkSchemaPublished(std::string_view schemaName) const {
      std::scoped_lock lock{m_entry->schemaMutex};
      m_entry->publishedSchemas.try_emplace(schemaName, true);
    }

   private:
    EntryHandle(std::string_view name, uint64_t resetGeneration,
                std::shared_ptr<TelemetryEntry> entry,
                std::shared_ptr<TelemetryBackend> backend)
        : m_entry{std::make_shared<CachedEntry>(
              name, resetGeneration, std::move(entry), std::move(backend))} {}

    std::shared_ptr<CachedEntry> m_entry;
  };

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
      if (!ShouldLogTableValue(name)) {
        return;
      }
      auto& table = GetTable(name);
      auto typeString = GetTelemetryTypeName(value, info...);
      if (!std::string_view{typeString}.empty() && !table.SetType(typeString)) {
        return;
      }
      LogTo(table, value, info...);
    } else if constexpr (SupportsTelemetry<T, I...>) {
      if (!ShouldLogTableValue(name)) {
        return;
      }
      auto& table = GetTable(name);
      LogTo(table, value, info...);
    } else if constexpr (SupportsTelemetryValue<T, I...>) {
      auto entry = GetEntry(name);
      if (entry->IsDiscard()) {
        return;
      }
      LogValueTo(*this, name, value, info...);
    } else if constexpr (std::constructible_from<std::string_view, T>) {
      auto entry = GetEntry(name);
      if (entry->IsDiscard()) {
        return;
      }
      if constexpr (sizeof...(I) == 0) {
        entry->LogString(std::string_view{value}, "string");
      } else if constexpr (sizeof...(I) == 1 &&
                           (std::constructible_from<std::string_view, I> &&
                            ...)) {
        entry->LogString(std::string_view{value}, std::string_view{info...});
      } else {
        static_assert(impl::always_false<T>::value,
                      "Don't know how to serialize type");
      }
    } else if constexpr (impl::IsSpanConvertible<T>) {
      if constexpr (impl::HasValueType<T>) {
        using V = typename std::remove_cvref_t<T>::value_type;
        Log(name, std::span<const V>{value}, info...);
      } else {
        using V = std::remove_extent_t<std::remove_cvref_t<T>>;
        Log(name, std::span<const V>{value}, info...);
      }
    } else if constexpr (wpi::util::StructSerializable<T, I...>) {
      auto entry = GetEntry(name);
      if (entry->IsDiscard()) {
        return;
      }
      auto path = std::format("{}{}", m_path, name);
      using S = wpi::util::Struct<T, I...>;
      auto typeString = wpi::util::GetStructTypeString<T>(info...);
      if (!PublishStructSchema<T>(entry, path, typeString, info...)) {
        return;
      }
      try {
        if constexpr (sizeof...(I) == 0) {
          if constexpr (wpi::util::is_constexpr([] { S::GetSize(); })) {
            uint8_t buf[S::GetSize()];
            S::Pack(buf, value);
            entry->LogRaw(std::span{buf}, typeString);
            return;
          }
        }
        wpi::util::SmallVector<uint8_t, 128> buf;
        buf.resize_for_overwrite(S::GetSize(info...));
        S::Pack(buf, value, info...);
        entry->LogRaw(std::span{buf}, typeString);
      } catch (const std::exception& e) {
        ReportWarning(path, "failed to publish struct value", e);
      } catch (...) {
        ReportWarning(path, "failed to publish struct value");
      }
    } else if constexpr (wpi::util::ProtobufSerializable<T>) {
      auto entry = GetEntry(name);
      if (entry->IsDiscard()) {
        return;
      }
      auto path = std::format("{}{}", m_path, name);
      wpi::util::ProtobufMessage<T> msg;
      auto typeString = msg.GetTypeString();
      if (!PublishProtobufSchema<T>(entry, path, typeString, msg)) {
        return;
      }
      wpi::util::SmallVector<uint8_t, 128> buf;
      try {
        if (!msg.Pack(buf, value)) {
          ReportWarning(path, "failed to publish protobuf value");
          return;
        }
        entry->LogRaw(buf, typeString);
      } catch (const std::exception& e) {
        ReportWarning(path, "failed to publish protobuf value", e);
      } catch (...) {
        ReportWarning(path, "failed to publish protobuf value");
      }
    } else if constexpr (std::integral<T>) {
      auto entry = GetEntry(name);
      if (entry->IsDiscard()) {
        return;
      }
      entry->LogInt64(static_cast<int64_t>(value));
    } else if constexpr (std::floating_point<T>) {
      auto entry = GetEntry(name);
      if (entry->IsDiscard()) {
        return;
      }
      entry->LogDouble(static_cast<double>(value));
    } else if constexpr (std::constructible_from<std::formatter<T>>) {
      auto entry = GetEntry(name);
      if (entry->IsDiscard()) {
        return;
      }
      entry->LogString(std::format("{}", value), "string");
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
    auto entry = GetEntry(name);
    if (entry->IsDiscard()) {
      return;
    }
    using U = std::remove_cv_t<T>;
    if constexpr (std::same_as<U, bool>) {
      entry->LogBooleanArray(value);
    } else if constexpr (std::same_as<U, int16_t>) {
      entry->LogInt16Array(value);
    } else if constexpr (std::same_as<U, int32_t>) {
      entry->LogInt32Array(value);
    } else if constexpr (std::same_as<U, int64_t>) {
      entry->LogInt64Array(value);
    } else if constexpr (std::same_as<U, float>) {
      entry->LogFloatArray(value);
    } else if constexpr (std::same_as<U, double>) {
      entry->LogDoubleArray(value);
    } else if constexpr (std::same_as<U, std::string>) {
      entry->LogStringArray(value);
    } else if constexpr (std::same_as<U, std::string_view>) {
      entry->LogStringArray(value);
    } else if constexpr (std::same_as<U, uint8_t>) {
      if constexpr (sizeof...(I) == 0) {
        entry->LogRaw(value, "raw");
      } else if constexpr (sizeof...(I) == 1 &&
                           (std::constructible_from<std::string_view, I> &&
                            ...)) {
        entry->LogRaw(value, std::string_view{info...});
      } else {
        static_assert(impl::always_false<T>::value,
                      "Don't know how to serialize type");
      }
    } else if constexpr (std::integral<U>) {
      std::vector<int64_t> values;
      values.reserve(value.size());
      for (auto&& v : value) {
        values.emplace_back(static_cast<int64_t>(v));
      }
      entry->LogInt64Array(values);
    } else if constexpr (wpi::util::StructSerializable<T, I...>) {
      auto structTypeString = wpi::util::GetStructTypeString<T>(info...);
      auto path = std::format("{}{}", m_path, name);
      if (!PublishStructSchema<T>(entry, path, structTypeString, info...)) {
        return;
      }
      try {
        wpi::util::StructArrayBuffer<T, I...> buf;
        buf.Write(
            value,
            [&](auto bytes) {
              entry->LogRaw(
                  bytes,
                  std::format("{}[]", std::string_view{structTypeString}));
            },
            info...);
      } catch (const std::exception& e) {
        ReportWarning(path, "failed to publish struct array value", e);
      } catch (...) {
        ReportWarning(path, "failed to publish struct array value");
      }
    } else if constexpr (std::constructible_from<std::formatter<T>>) {
      std::vector<std::string> strings;
      strings.reserve(value.size());
      for (auto&& v : value) {
        strings.emplace_back(std::format("{}", v));
      }
      entry->LogStringArray(strings);
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

  /**
   * Returns whether a table-valued object should be expanded for logging.
   * Returns false when the entry for name discards data and no registered
   * descendant backend can receive data under the child table.
   *
   * @param name the name
   * @return True if table-valued logging should expand
   */
  bool ShouldLogTableValue(std::string_view name);

 private:
  /**
   * Gets a telemetry entry.
   *
   * @param name name
   * @return entry
   */
  EntryHandle GetEntry(std::string_view name);

  EntryHandle GetEntry(std::string_view name, bool* metadataApplied);

  bool HasNonDiscardDescendant();

  void TypeMismatch(std::string_view expectedType, std::string_view typeString);

  template <typename T, typename... I>
    requires wpi::util::StructSerializable<T, I...>
  bool PublishStructSchema(const EntryHandle& entry, std::string_view path,
                           std::string_view typeString, const I&... info) {
    if (entry.HasPublishedSchema(typeString)) {
      return true;
    }
    try {
      TelemetryRegistry::AddStructSchema<T>(entry.GetBackend(), info...);
    } catch (const std::exception& e) {
      ReportWarning(path, "failed to publish struct schema", e);
      return false;
    } catch (...) {
      ReportWarning(path, "failed to publish struct schema");
      return false;
    }
    entry.MarkSchemaPublished(typeString);
    return true;
  }

  template <wpi::util::ProtobufSerializable T>
  bool PublishProtobufSchema(const EntryHandle& entry, std::string_view path,
                             std::string_view typeString,
                             wpi::util::ProtobufMessage<T>& msg) {
    if (entry.HasPublishedSchema(typeString)) {
      return true;
    }
    try {
      TelemetryRegistry::AddProtobufSchema<T>(entry.GetBackend(), msg);
    } catch (const std::exception& e) {
      ReportWarning(path, "failed to publish protobuf schema", e);
      return false;
    } catch (...) {
      ReportWarning(path, "failed to publish protobuf schema");
      return false;
    }
    entry.MarkSchemaPublished(typeString);
    return true;
  }

  static void ReportWarning(std::string_view path, std::string_view msg,
                            const std::exception& e) {
    TelemetryRegistry::ReportWarning(path,
                                     std::format("{}: {}", msg, e.what()));
  }

  static void ReportWarning(std::string_view path, std::string_view msg) {
    TelemetryRegistry::ReportWarning(path, msg);
  }

  /** Clears the table's cached entries. */
  void Reset();

  mutable wpi::util::mutex m_mutex;
  std::string m_path;
  mutable wpi::util::StringMap<TelemetryTable*> m_tableAliasesMap;
  mutable wpi::util::StringMap<TelemetryTable*> m_tablesMap;
  mutable wpi::util::StringMap<EntryHandle> m_entriesMap;
  std::string m_type;
  bool m_hasNonDiscardDescendant = false;
  bool m_hasNonDiscardDescendantCached = false;
  uint64_t m_resetGeneration = 0;
};

}  // namespace wpi::telemetry
