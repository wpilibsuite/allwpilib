// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/datalog/DataLogTelemetryBackend.h"

#include <atomic>
#include <string>
#include <string_view>
#include <variant>

#include <fmt/format.h>

#include "wpi/mutex.h"
#include "wpi/telemetry/TelemetryEntry.h"
#include "wpi/DataLog.h"

using namespace wpi;

class DataLogTelemetryBackend::Entry : public TelemetryEntry {
 public:
  Entry(wpi::log::DataLog& log, std::string_view prefix, std::string_view path)
      : m_log{log}, m_path{fmt::format("{}{}", prefix, path)} {}

  void KeepDuplicates() override { m_keepDuplicates = true; }

  void SetProperty(std::string_view key, std::string_view value) override {
    std::scoped_lock lock{m_mutex};
    auto& curValue = m_propertiesMap[key];
    if (curValue == value) {
      return;
    }
    curValue = value;
    // TODO
  }

  template <typename EntryType, typename T>
  void Log(T value) {
    std::scoped_lock lock{m_mutex};
    if (std::holds_alternative<std::monostate>(m_entry)) {
      m_entry = EntryType{m_log, m_path, m_properties};
    }
    if (auto entry = std::get_if<EntryType>(&m_entry)) {
      if (m_keepDuplicates) {
        entry->Append(value);
      } else {
        entry->Update(value);
      }
    } else {
      // TODO: warn?
    }
  }

  template <typename EntryType, typename T>
  void LogTypeString(T value, std::string_view typeString) {
    std::scoped_lock lock{m_mutex};
    if (std::holds_alternative<std::monostate>(m_entry)) {
      m_typeString = typeString;
      m_entry = EntryType{m_log, m_path, m_properties, m_typeString};
    }
    if (auto entry = std::get_if<EntryType>(&m_entry);
        entry && m_typeString == typeString) {
      if (m_keepDuplicates) {
        entry->Append(value);
      } else {
        entry->Update(value);
      }
    } else {
      // TODO: warn?
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
  wpi::mutex m_mutex;
  std::variant<std::monostate, wpi::log::BooleanLogEntry,
               wpi::log::IntegerLogEntry, wpi::log::FloatLogEntry,
               wpi::log::DoubleLogEntry, wpi::log::StringLogEntry,
               wpi::log::BooleanArrayLogEntry, wpi::log::IntegerArrayLogEntry,
               wpi::log::FloatArrayLogEntry, wpi::log::DoubleArrayLogEntry,
               wpi::log::StringArrayLogEntry, wpi::log::RawLogEntry>
      m_entry;
  std::string m_typeString;
  std::atomic_bool m_keepDuplicates{false};
  wpi::StringMap<std::string> m_propertiesMap;
  std::string m_properties = "{}";
};

DataLogTelemetryBackend::DataLogTelemetryBackend(wpi::log::DataLog& log,
                                                 std::string_view prefix)
    : m_log{log}, m_prefix{prefix} {}

DataLogTelemetryBackend::~DataLogTelemetryBackend() = default;

TelemetryEntry& DataLogTelemetryBackend::GetEntry(std::string_view path) {
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
