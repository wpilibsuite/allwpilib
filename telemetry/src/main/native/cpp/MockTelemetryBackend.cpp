// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/MockTelemetryBackend.h"

#include <string>
#include <vector>

#include "wpi/telemetry/TelemetryEntry.h"

using namespace wpi;

class MockTelemetryBackend::Entry : public TelemetryEntry {
 public:
  Entry(std::string_view path, MockTelemetryBackend& backend)
      : m_path{path}, m_backend{backend} {}

  void KeepDuplicates() override {
    m_backend.m_actions.emplace_back(m_path, KeepDuplicatesValue{true});
  }

  void SetProperty(std::string_view key, std::string_view value) override {
    m_backend.m_actions.emplace_back(
        m_path, SetPropertyValue{std::string{key}, std::string{value}});
  }

  void LogBoolean(bool value) override {
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogInt16(int16_t value) override {
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogInt32(int32_t value) override {
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogInt64(int64_t value) override {
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogFloat(float value) override {
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogDouble(double value) override {
    m_backend.m_actions.emplace_back(m_path, value);
  }

  void LogString(std::string_view value, std::string_view typeString) override {
    m_backend.m_actions.emplace_back(
        m_path, LogStringValue{std::string{value}, std::string{typeString}});
  }

  void LogBooleanArray(std::span<const bool> value) override {
    m_backend.m_actions.emplace_back(
        m_path,
        LogBooleanArrayValue{std::vector<int>{value.begin(), value.end()}});
  }

  void LogBooleanArray(std::span<const int> value) override {
    m_backend.m_actions.emplace_back(
        m_path,
        LogBooleanArrayValue{std::vector<int>{value.begin(), value.end()}});
  }

  void LogInt16Array(std::span<const int16_t> value) override {
    m_backend.m_actions.emplace_back(
        m_path, std::vector<int16_t>{value.begin(), value.end()});
  }

  void LogInt32Array(std::span<const int32_t> value) override {
    m_backend.m_actions.emplace_back(
        m_path, std::vector<int32_t>{value.begin(), value.end()});
  }

  void LogInt64Array(std::span<const int64_t> value) override {
    m_backend.m_actions.emplace_back(
        m_path, std::vector<int64_t>{value.begin(), value.end()});
  }

  void LogFloatArray(std::span<const float> value) override {
    m_backend.m_actions.emplace_back(
        m_path, std::vector<float>{value.begin(), value.end()});
  }

  void LogDoubleArray(std::span<const double> value) override {
    m_backend.m_actions.emplace_back(
        m_path, std::vector<double>{value.begin(), value.end()});
  }

  void LogStringArray(std::span<const std::string> value) override {
    m_backend.m_actions.emplace_back(
        m_path, std::vector<std::string>{value.begin(), value.end()});
  }

  void LogStringArray(std::span<const std::string_view> value) override {
    m_backend.m_actions.emplace_back(
        m_path, std::vector<std::string>{value.begin(), value.end()});
  }

  void LogRaw(std::span<const uint8_t> value,
              std::string_view typeString) override {
    m_backend.m_actions.emplace_back(
        m_path, LogRawValue{std::vector<uint8_t>{value.begin(), value.end()},
                            std::string{typeString}});
  }

 private:
  std::string m_path;
  MockTelemetryBackend& m_backend;
};

MockTelemetryBackend::MockTelemetryBackend() = default;

MockTelemetryBackend::~MockTelemetryBackend() = default;

TelemetryEntry& MockTelemetryBackend::GetEntry(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  return m_entries.try_emplace(path, path, *this).first->second;
}

void MockTelemetryBackend::SetReportWarning(
    std::function<void(std::string_view msg)> func) {}

bool MockTelemetryBackend::HasSchema(std::string_view schemaName) const {
  return m_schemas.contains(schemaName);
}

void MockTelemetryBackend::AddSchema(std::string_view schemaName,
                                     std::string_view type,
                                     std::span<const uint8_t> schema) {
  auto& val = m_schemas[schemaName];
  val.type = type;
  val.schemaBytes.assign(schema.begin(), schema.end());
}

void MockTelemetryBackend::AddSchema(std::string_view schemaName,
                                     std::string_view type,
                                     std::string_view schema) {
  auto& val = m_schemas[schemaName];
  val.type = type;
  val.schemaString = schema;
}
