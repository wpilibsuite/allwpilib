// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/MockTelemetryBackend.hpp"

#include <string>
#include <vector>

#include "wpi/telemetry/TelemetryEntry.hpp"

using namespace wpi;

class MockTelemetryBackend::Entry : public TelemetryEntry {
 public:
  Entry(std::string_view path, MockTelemetryBackend& backend)
      : m_path{path}, m_backend{backend} {}

  int GetLast() const { return m_last; }
  void ClearLast() { m_last = -1; }

  void KeepDuplicates() override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, KeepDuplicatesValue{true});
  }

  void SetProperty(std::string_view key, std::string_view value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, SetPropertyValue{std::string{key}, std::string{value}});
  }

  void LogBoolean(bool value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogInt16(int16_t value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogInt32(int32_t value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogInt64(int64_t value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogFloat(float value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogDouble(double value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogString(std::string_view value, std::string_view typeString) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, LogStringValue{std::string{value}, std::string{typeString}});
  }

  void LogBooleanArray(std::span<const bool> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path,
        LogBooleanArrayValue{std::vector<int>{value.begin(), value.end()}});
  }

  void LogBooleanArray(std::span<const int> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path,
        LogBooleanArrayValue{std::vector<int>{value.begin(), value.end()}});
  }

  void LogInt16Array(std::span<const int16_t> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, std::vector<int16_t>{value.begin(), value.end()});
  }

  void LogInt32Array(std::span<const int32_t> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, std::vector<int32_t>{value.begin(), value.end()});
  }

  void LogInt64Array(std::span<const int64_t> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, std::vector<int64_t>{value.begin(), value.end()});
  }

  void LogFloatArray(std::span<const float> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, std::vector<float>{value.begin(), value.end()});
  }

  void LogDoubleArray(std::span<const double> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, std::vector<double>{value.begin(), value.end()});
  }

  void LogStringArray(std::span<const std::string> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, std::vector<std::string>{value.begin(), value.end()});
  }

  void LogStringArray(std::span<const std::string_view> value) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, std::vector<std::string>{value.begin(), value.end()});
  }

  void LogRaw(std::span<const uint8_t> value,
              std::string_view typeString) override {
    std::scoped_lock lock{m_backend.m_mutex};
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(
        m_path, LogRawValue{std::vector<uint8_t>{value.begin(), value.end()},
                            std::string{typeString}});
  }

 private:
  std::string m_path;
  MockTelemetryBackend& m_backend;
  int m_last = -1;
};

MockTelemetryBackend::MockTelemetryBackend() = default;

MockTelemetryBackend::~MockTelemetryBackend() = default;

void MockTelemetryBackend::Clear() {
  std::scoped_lock lock{m_mutex};
  for (auto&& kv : m_entries) {
    kv.second.ClearLast();
  }
  m_actions.clear();
}

TelemetryEntry& MockTelemetryBackend::GetEntry(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  return m_entries.try_emplace(path, path, *this).first->second;
}

bool MockTelemetryBackend::HasSchema(std::string_view schemaName) const {
  std::scoped_lock lock{m_mutex};
  return m_schemas.contains(schemaName);
}

void MockTelemetryBackend::AddSchema(std::string_view schemaName,
                                     std::string_view type,
                                     std::span<const uint8_t> schema) {
  std::scoped_lock lock{m_mutex};
  auto& val = m_schemas[schemaName];
  val.type = type;
  val.schemaBytes.assign(schema.begin(), schema.end());
}

void MockTelemetryBackend::AddSchema(std::string_view schemaName,
                                     std::string_view type,
                                     std::string_view schema) {
  std::scoped_lock lock{m_mutex};
  auto& val = m_schemas[schemaName];
  val.type = type;
  val.schemaString = schema;
}

const MockTelemetryBackend::Action* MockTelemetryBackend::GetLastActionImpl(
    std::string_view path) const {
  auto it = m_entries.find(path);
  if (it == m_entries.end()) {
    return nullptr;
  }
  int n = it->second.GetLast();
  if (n == -1) {
    return nullptr;
  }
  return &m_actions[n];
}
