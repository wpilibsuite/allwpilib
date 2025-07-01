// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryTable.hpp"

#include <format>
#include <string>

#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"

using namespace wpi;

std::string_view TelemetryTable::GetPath() const {
  return m_path;
}

bool TelemetryTable::SetType(std::string_view typeString) {
  {
    std::scoped_lock lock{m_mutex};
    if (!m_type.empty()) {
      if (m_type == typeString) {
        return true;
      }
      TypeMismatch(typeString);
      return false;
    }
    m_type = typeString;
  }
  Log(".type", typeString);
  return true;
}

std::string TelemetryTable::GetType() const {
  std::scoped_lock lock{m_mutex};
  return m_type;
}

bool TelemetryTable::HasType() const {
  std::scoped_lock lock{m_mutex};
  return !m_type.empty();
}

TelemetryTable& TelemetryTable::GetTable(std::string_view name) const {
  std::unique_lock lock{m_mutex};
  TelemetryTable* table = m_tablesMap[name];
  if (table) {
    return *table;
  }
  lock.unlock();  // avoid lock inversion
  TelemetryTable* newTable =
      &TelemetryRegistry::GetTable(std::format("{}{}/", m_path, name));
  lock.lock();
  auto& table2 = m_tablesMap[name];
  if (!table2) {
    table2 = newTable;
  }
  return *table2;
}

void TelemetryTable::KeepDuplicates(std::string_view name) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.KeepDuplicates();
}

void TelemetryTable::SetProperty(std::string_view name, std::string_view key,
                                 std::string_view value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.SetProperty(key, value);
}

void TelemetryTable::Log(std::string_view name, bool value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogBoolean(value);
}

void TelemetryTable::Log(std::string_view name, int8_t value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogInt8(value);
}

void TelemetryTable::Log(std::string_view name, int16_t value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogInt16(value);
}

void TelemetryTable::Log(std::string_view name, int32_t value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogInt32(value);
}

void TelemetryTable::Log(std::string_view name, int64_t value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogInt64(value);
}

void TelemetryTable::Log(std::string_view name, float value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogFloat(value);
}

void TelemetryTable::Log(std::string_view name, double value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogDouble(value);
}

void TelemetryTable::Log(std::string_view name, std::string_view value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogString(value, "string");
}

void TelemetryTable::Log(std::string_view name, std::string_view value,
                         std::string_view typeString) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogString(value, typeString);
}

void TelemetryTable::Log(std::string_view name, std::span<const bool> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogBooleanArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int16_t> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogInt16Array(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int32_t> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogInt32Array(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int64_t> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogInt64Array(value);
}

void TelemetryTable::Log(std::string_view name, std::span<const float> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogFloatArray(value);
}

void TelemetryTable::Log(std::string_view name, std::span<const double> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogDoubleArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const std::string> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogStringArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const std::string_view> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogStringArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const uint8_t> value) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogRaw(value, "raw");
}

void TelemetryTable::Log(std::string_view name, std::span<const uint8_t> value,
                         std::string_view typeString) {
  TelemetryEntry& entry = GetEntry(name);
  if (entry.IsDiscard()) {
    return;
  }
  entry.LogRaw(value, typeString);
}

TelemetryEntry& TelemetryTable::GetEntry(std::string_view name) {
  std::unique_lock lock{m_mutex};
  TelemetryEntry* entry = m_entriesMap[name];
  if (entry) {
    return *entry;
  }
  lock.unlock();  // avoid lock inversion
  TelemetryEntry* newEntry =
      &TelemetryRegistry::GetEntry(std::format("{}{}", m_path, name));
  lock.lock();
  auto& entry2 = m_entriesMap[name];
  if (!entry2) {
    entry2 = newEntry;
  }
  return *entry2;
}

void TelemetryTable::TypeMismatch(std::string_view typeString) {
  TelemetryRegistry::ReportWarning(
      m_path, std::format("table type mismatch, expected '{}', got '{}'",
                          m_type, typeString));
}

void TelemetryTable::Reset() {
  std::scoped_lock lock{m_mutex};
  m_tablesMap.clear();
  m_entriesMap.clear();
}
