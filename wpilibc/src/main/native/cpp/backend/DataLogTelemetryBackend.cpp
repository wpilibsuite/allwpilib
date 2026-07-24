// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/DataLogTelemetryBackend.hpp"

#include <atomic>
#include <format>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include "wpi/datalog/DataLog.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi::backend;

class DataLogTelemetryBackend::Entry : public wpi::TelemetryEntry {
 public:
  Entry(wpi::log::DataLog& log, std::string_view prefix, std::string_view path)
      : m_log{log}, m_path{std::format("{}{}", prefix, path)} {}

  void KeepDuplicates() override { m_keepDuplicates = true; }

  void SetProperty(std::string_view key, std::string_view value) override {
    std::scoped_lock lock{m_mutex};
    auto& propMap = m_properties.get_object();
    auto it = propMap.find(key);
    if (it == propMap.end()) {
      propMap.emplace(key, value);
    } else {
      auto& curVal = it->second.get_string();
      if (curVal == value) {
        return;
      }
      curVal = value;
    }
    m_propertiesStr = m_properties.to_string();
    if (m_entryIndex != 0) {
      m_log.SetMetadata(m_entryIndex, m_propertiesStr);
    }
  }

  template <typename EntryType, typename T>
  void Log(T value) {
    std::scoped_lock lock{m_mutex};
    if (std::holds_alternative<std::monostate>(m_entry)) {
      EntryType entry{m_log, m_path, m_propertiesStr};
      m_entryIndex = entry.GetIndex();
      m_entry = std::move(entry);
    }
    if (auto entry = std::get_if<EntryType>(&m_entry)) {
      if (m_keepDuplicates) {
        entry->Append(value);
      } else {
        entry->Update(value);
      }
    } else {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  template <typename EntryType, typename T>
  void LogTypeString(T value, std::string_view typeString) {
    std::scoped_lock lock{m_mutex};
    if (std::holds_alternative<std::monostate>(m_entry)) {
      m_typeString = typeString;
      EntryType entry{m_log, m_path, m_propertiesStr, m_typeString};
      m_entryIndex = entry.GetIndex();
      m_entry = std::move(entry);
    }
    if (auto entry = std::get_if<EntryType>(&m_entry);
        entry && m_typeString == typeString) {
      if (m_keepDuplicates) {
        entry->Append(value);
      } else {
        entry->Update(value);
      }
    } else {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogBoolean(bool value) override {
    Log<wpi::log::BooleanLogEntry>(value);
  }

  void LogInt64(int64_t value) override {
    Log<wpi::log::IntegerLogEntry>(value);
  }

  void LogFloat(float value) override { Log<wpi::log::FloatLogEntry>(value); }

  void LogDouble(double value) override {
    Log<wpi::log::DoubleLogEntry>(value);
  }

  void LogString(std::string_view value, std::string_view typeString) override {
    LogTypeString<wpi::log::StringLogEntry>(value, typeString);
  }

  void LogBooleanArray(std::span<const bool> value) override {
    Log<wpi::log::BooleanArrayLogEntry>(value);
  }

  void LogBooleanArray(std::span<const int> value) override {
    Log<wpi::log::BooleanArrayLogEntry>(value);
  }

  void LogInt16Array(std::span<const int16_t> value) override {
    // TODO
  }

  void LogInt32Array(std::span<const int32_t> value) override {
    // TODO
  }

  void LogInt64Array(std::span<const int64_t> value) override {
    Log<wpi::log::IntegerArrayLogEntry>(value);
  }

  void LogFloatArray(std::span<const float> value) override {
    Log<wpi::log::FloatArrayLogEntry>(value);
  }

  void LogDoubleArray(std::span<const double> value) override {
    Log<wpi::log::DoubleArrayLogEntry>(value);
  }

  void LogStringArray(std::span<const std::string> value) override {
    Log<wpi::log::StringArrayLogEntry>(value);
  }

  void LogStringArray(std::span<const std::string_view> value) override {
    Log<wpi::log::StringArrayLogEntry>(value);
  }

  void LogRaw(std::span<const uint8_t> value,
              std::string_view typeString) override {
    LogTypeString<wpi::log::RawLogEntry>(value, typeString);
  }

 private:
  wpi::log::DataLog& m_log;
  std::string m_path;
  wpi::util::mutex m_mutex;
  int m_entryIndex = 0;
  std::variant<std::monostate, wpi::log::BooleanLogEntry,
               wpi::log::IntegerLogEntry, wpi::log::FloatLogEntry,
               wpi::log::DoubleLogEntry, wpi::log::StringLogEntry,
               wpi::log::BooleanArrayLogEntry, wpi::log::IntegerArrayLogEntry,
               wpi::log::FloatArrayLogEntry, wpi::log::DoubleArrayLogEntry,
               wpi::log::StringArrayLogEntry, wpi::log::RawLogEntry>
      m_entry;
  std::string m_typeString;
  std::atomic_bool m_keepDuplicates{false};
  wpi::util::json m_properties;
  std::string m_propertiesStr = "{}";
};

DataLogTelemetryBackend::DataLogTelemetryBackend(wpi::log::DataLog& log,
                                                 std::string_view prefix)
    : m_log{log}, m_prefix{prefix} {}

DataLogTelemetryBackend::~DataLogTelemetryBackend() = default;

wpi::TelemetryEntry& DataLogTelemetryBackend::GetEntry(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  return m_entries.try_emplace(path, m_log, m_prefix, path).first->second;
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
