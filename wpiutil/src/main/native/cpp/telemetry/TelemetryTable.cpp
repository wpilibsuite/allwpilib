// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryTable.h"

#include <fmt/format.h>

#include "wpi/telemetry/TelemetryEntry.h"
#include "wpi/telemetry/TelemetryRegistry.h"

using namespace wpi;

std::string_view TelemetryTable::GetPath() const {
  return m_path;
}

TelemetryTable& TelemetryTable::GetTable(std::string_view name) const {
  std::scoped_lock lock{m_mutex};
  auto& table = m_tablesMap[name];
  if (!table) {
    table = &TelemetryRegistry::GetTable(fmt::format("{}{}/", m_path, name));
  }
  return *table;
}

void TelemetryTable::KeepDuplicates(std::string_view name) {
  GetEntry(name).KeepDuplicates();
}

void TelemetryTable::SetProperty(std::string_view name, std::string_view key,
                                 std::string_view value) {
  GetEntry(name).SetProperty(key, value);
}

void TelemetryTable::SetTypeString(std::string_view name,
                                   std::string_view typeString) {
  GetEntry(name).SetTypeString(typeString);
}

void TelemetryTable::Log(std::string_view name, bool value) {
  GetEntry(name).LogBoolean(value);
}

void TelemetryTable::Log(std::string_view name, int8_t value) {
  GetEntry(name).LogInt8(value);
}

void TelemetryTable::Log(std::string_view name, int16_t value) {
  GetEntry(name).LogInt16(value);
}

void TelemetryTable::Log(std::string_view name, int32_t value) {
  GetEntry(name).LogInt32(value);
}

void TelemetryTable::Log(std::string_view name, int64_t value) {
  GetEntry(name).LogInt64(value);
}

void TelemetryTable::Log(std::string_view name, float value) {
  GetEntry(name).LogFloat(value);
}

void TelemetryTable::Log(std::string_view name, double value) {
  GetEntry(name).LogDouble(value);
}

void TelemetryTable::Log(std::string_view name, std::string_view value) {
  GetEntry(name).LogString(value);
}

void TelemetryTable::Log(std::string_view name, std::span<const bool> value) {
  GetEntry(name).LogBooleanArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const uint8_t> value) {
  GetEntry(name).LogByteArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int16_t> value) {
  GetEntry(name).LogInt16Array(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int32_t> value) {
  GetEntry(name).LogInt32Array(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const int64_t> value) {
  GetEntry(name).LogInt64Array(value);
}

void TelemetryTable::Log(std::string_view name, std::span<const float> value) {
  GetEntry(name).LogFloatArray(value);
}

void TelemetryTable::Log(std::string_view name, std::span<const double> value) {
  GetEntry(name).LogDoubleArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const std::string> value) {
  GetEntry(name).LogStringArray(value);
}

void TelemetryTable::Log(std::string_view name,
                         std::span<const std::string_view> value) {
  GetEntry(name).LogStringArray(value);
}

TelemetryEntry& TelemetryTable::GetEntry(std::string_view name) {
  std::scoped_lock lock{m_mutex};
  auto& entry = m_entriesMap[name];
  if (!entry) {
    entry = &TelemetryRegistry::GetEntry(fmt::format("{}{}", m_path, name));
  }
  return *entry;
}

void TelemetryTable::Reset() {
  std::scoped_lock lock{m_mutex};
  m_tablesMap.clear();
  m_entriesMap.clear();
}
