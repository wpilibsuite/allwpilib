// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/DataLogTelemetryBackend.hpp"

#include <atomic>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "wpi/datalog/DataLog.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi::backend;

class DataLogTelemetryBackend::Entry : public wpi::telemetry::TelemetryEntry {
 public:
  Entry(wpi::log::DataLog& log, std::string_view prefix, std::string_view path)
      : m_log{log}, m_path{std::format("{}{}", prefix, path)} {}

  void MarkRemoved() {
    std::scoped_lock lock{m_mutex};
    std::visit(
        [](auto& entry) {
          using EntryType = std::decay_t<decltype(entry)>;
          if constexpr (!std::is_same_v<EntryType, std::monostate>) {
            entry.Finish();
          }
        },
        m_entry);
    m_entry = std::monostate{};
    m_entryIndex = 0;
    m_removed.store(true);
  }

  bool IsDiscard() const override { return m_removed.load(); }

  void KeepDuplicates() override {
    if (!m_removed.load()) {
      m_keepDuplicates = true;
    }
  }

  void SetProperty(std::string_view key, std::string_view value) override {
    if (m_removed.load()) {
      return;
    }
    auto parsedValue = wpi::util::json::parse(value);
    if (!parsedValue) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path,
                                                       "invalid property JSON");
      return;
    }

    std::scoped_lock lock{m_mutex};
    if (m_removed.load()) {
      return;
    }
    auto& propMap = m_properties.get_object();
    auto it = propMap.find(key);
    if (it == propMap.end()) {
      propMap.emplace(key, std::move(*parsedValue));
    } else {
      if (it->second == *parsedValue) {
        return;
      }
      it->second = std::move(*parsedValue);
    }
    m_propertiesStr = m_properties.to_string();
    if (m_entryIndex != 0) {
      m_log.SetMetadata(m_entryIndex, m_propertiesStr);
    }
  }

  template <typename EntryType, typename T>
  void Log(T value, int64_t timestamp) {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (std::holds_alternative<std::monostate>(m_entry)) {
        EntryType entry{m_log, m_path, m_propertiesStr, timestamp};
        m_entryIndex = entry.GetIndex();
        m_entry = std::move(entry);
      }
      if (auto entry = std::get_if<EntryType>(&m_entry)) {
        if (m_keepDuplicates) {
          entry->Append(value, timestamp);
        } else {
          entry->Update(value, timestamp);
        }
      } else {
        typeMismatch = true;
      }
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  template <typename EntryType, typename T>
  void LogTypeString(T value, std::string_view typeString, int64_t timestamp) {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (std::holds_alternative<std::monostate>(m_entry)) {
        m_typeString = typeString;
        EntryType entry{m_log, m_path, m_propertiesStr, m_typeString,
                        timestamp};
        m_entryIndex = entry.GetIndex();
        m_entry = std::move(entry);
      }
      if (auto entry = std::get_if<EntryType>(&m_entry);
          entry && m_typeString == typeString) {
        if (m_keepDuplicates) {
          entry->Append(value, timestamp);
        } else {
          entry->Update(value, timestamp);
        }
      } else {
        typeMismatch = true;
      }
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogBoolean(bool value, int64_t timestamp) override {
    Log<wpi::log::BooleanLogEntry>(value, timestamp);
  }

  void LogInt64(int64_t value, int64_t timestamp) override {
    Log<wpi::log::IntegerLogEntry>(value, timestamp);
  }

  void LogFloat(float value, int64_t timestamp) override {
    Log<wpi::log::FloatLogEntry>(value, timestamp);
  }

  void LogDouble(double value, int64_t timestamp) override {
    Log<wpi::log::DoubleLogEntry>(value, timestamp);
  }

  void LogString(std::string_view value, std::string_view typeString,
                 int64_t timestamp) override {
    LogTypeString<wpi::log::StringLogEntry>(value, typeString, timestamp);
  }

  void LogBooleanArray(std::span<const bool> value,
                       int64_t timestamp) override {
    Log<wpi::log::BooleanArrayLogEntry>(value, timestamp);
  }

  void LogBooleanArray(std::span<const int> value, int64_t timestamp) override {
    Log<wpi::log::BooleanArrayLogEntry>(value, timestamp);
  }

  void LogInt16Array(std::span<const int16_t> value,
                     int64_t timestamp) override {
    std::vector<int64_t> arr{value.begin(), value.end()};
    LogInt64Array(arr, timestamp);
  }

  void LogInt32Array(std::span<const int32_t> value,
                     int64_t timestamp) override {
    std::vector<int64_t> arr{value.begin(), value.end()};
    LogInt64Array(arr, timestamp);
  }

  void LogInt64Array(std::span<const int64_t> value,
                     int64_t timestamp) override {
    Log<wpi::log::IntegerArrayLogEntry>(value, timestamp);
  }

  void LogFloatArray(std::span<const float> value, int64_t timestamp) override {
    Log<wpi::log::FloatArrayLogEntry>(value, timestamp);
  }

  void LogDoubleArray(std::span<const double> value,
                      int64_t timestamp) override {
    Log<wpi::log::DoubleArrayLogEntry>(value, timestamp);
  }

  void LogStringArray(std::span<const std::string> value,
                      int64_t timestamp) override {
    Log<wpi::log::StringArrayLogEntry>(value, timestamp);
  }

  void LogStringArray(std::span<const std::string_view> value,
                      int64_t timestamp) override {
    Log<wpi::log::StringArrayLogEntry>(value, timestamp);
  }

  void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
              int64_t timestamp) override {
    LogTypeString<wpi::log::RawLogEntry>(value, typeString, timestamp);
  }

 private:
  wpi::log::DataLog& m_log;
  std::string m_path;
  wpi::util::mutex m_mutex;
  int m_entryIndex = 0;
  std::atomic_bool m_removed{false};
  std::variant<std::monostate, wpi::log::BooleanLogEntry,
               wpi::log::IntegerLogEntry, wpi::log::FloatLogEntry,
               wpi::log::DoubleLogEntry, wpi::log::StringLogEntry,
               wpi::log::BooleanArrayLogEntry, wpi::log::IntegerArrayLogEntry,
               wpi::log::FloatArrayLogEntry, wpi::log::DoubleArrayLogEntry,
               wpi::log::StringArrayLogEntry, wpi::log::RawLogEntry>
      m_entry;
  std::string m_typeString;
  std::atomic_bool m_keepDuplicates{false};
  wpi::util::json m_properties = wpi::util::json::object();
  std::string m_propertiesStr = "{}";
};

DataLogTelemetryBackend::DataLogTelemetryBackend(wpi::log::DataLog& log,
                                                 std::string_view prefix)
    : m_log{log}, m_prefix{prefix} {}

DataLogTelemetryBackend::~DataLogTelemetryBackend() = default;

std::shared_ptr<wpi::telemetry::TelemetryEntry>
DataLogTelemetryBackend::GetEntry(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    return it->second;
  }

  auto entry = std::make_shared<Entry>(m_log, m_prefix, path);
  m_entries.try_emplace(path, entry);
  return entry;
}

void DataLogTelemetryBackend::RemoveEntry(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    auto entry = std::move(it->second);
    m_entries.erase(it);
    entry->MarkRemoved();
  }
}

bool DataLogTelemetryBackend::HasSchema(std::string_view schemaName) const {
  return m_log.HasSchema(schemaName);
}

void DataLogTelemetryBackend::AddSchema(std::string_view schemaName,
                                        std::string_view type,
                                        std::span<const uint8_t> schema) {
  m_log.AddSchema(schemaName, type, schema);
}

void DataLogTelemetryBackend::AddSchema(std::string_view schemaName,
                                        std::string_view type,
                                        std::string_view schema) {
  m_log.AddSchema(schemaName, type, schema);
}
