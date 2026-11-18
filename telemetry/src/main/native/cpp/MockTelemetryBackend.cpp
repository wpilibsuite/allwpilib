// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/MockTelemetryBackend.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/detail/PathUtil.hpp"

using namespace wpi;
using namespace wpi::telemetry;

class MockTelemetryBackend::Entry : public TelemetryEntry {
 public:
  Entry(std::string_view path, MockTelemetryBackend& backend)
      : m_path{path}, m_backend{backend} {}

  int GetLast() const { return m_last; }
  void ClearLast() { m_last = -1; }
  void MarkRemoved() {
    m_removed = true;
    m_last = -1;
  }

  bool IsDiscard() const override {
    std::scoped_lock lock{m_backend.m_mutex};
    return m_removed;
  }

  void KeepDuplicates() override { AppendAction(KeepDuplicatesValue{true}); }

  void SetProperty(std::string_view key, std::string_view value) override {
    AppendAction(SetPropertyValue{std::string{key}, std::string{value}});
  }

  void LogBoolean(bool value, int64_t timestamp) override {
    AppendAction(value, timestamp);
  }

  void LogInt16(int16_t value, int64_t timestamp) override {
    AppendAction(value, timestamp);
  }

  void LogInt32(int32_t value, int64_t timestamp) override {
    AppendAction(value, timestamp);
  }

  void LogInt64(int64_t value, int64_t timestamp) override {
    AppendAction(value, timestamp);
  }

  void LogFloat(float value, int64_t timestamp) override {
    AppendAction(value, timestamp);
  }

  void LogDouble(double value, int64_t timestamp) override {
    AppendAction(value, timestamp);
  }

  void LogString(std::string_view value, std::string_view typeString,
                 int64_t timestamp) override {
    AppendAction(LogStringValue{std::string{value}, std::string{typeString}},
                 timestamp);
  }

  void LogBooleanArray(std::span<const bool> value,
                       int64_t timestamp) override {
    AppendAction(
        LogBooleanArrayValue{std::vector<int>{value.begin(), value.end()}},
        timestamp);
  }

  void LogBooleanArray(std::span<const int> value, int64_t timestamp) override {
    AppendAction(
        LogBooleanArrayValue{std::vector<int>{value.begin(), value.end()}},
        timestamp);
  }

  void LogInt16Array(std::span<const int16_t> value,
                     int64_t timestamp) override {
    AppendAction(std::vector<int16_t>{value.begin(), value.end()}, timestamp);
  }

  void LogInt32Array(std::span<const int32_t> value,
                     int64_t timestamp) override {
    AppendAction(std::vector<int32_t>{value.begin(), value.end()}, timestamp);
  }

  void LogInt64Array(std::span<const int64_t> value,
                     int64_t timestamp) override {
    AppendAction(std::vector<int64_t>{value.begin(), value.end()}, timestamp);
  }

  void LogFloatArray(std::span<const float> value, int64_t timestamp) override {
    AppendAction(std::vector<float>{value.begin(), value.end()}, timestamp);
  }

  void LogDoubleArray(std::span<const double> value,
                      int64_t timestamp) override {
    AppendAction(std::vector<double>{value.begin(), value.end()}, timestamp);
  }

  void LogStringArray(std::span<const std::string> value,
                      int64_t timestamp) override {
    AppendAction(std::vector<std::string>{value.begin(), value.end()},
                 timestamp);
  }

  void LogStringArray(std::span<const std::string_view> value,
                      int64_t timestamp) override {
    AppendAction(std::vector<std::string>{value.begin(), value.end()},
                 timestamp);
  }

  void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
              int64_t timestamp) override {
    AppendAction(LogRawValue{std::vector<uint8_t>{value.begin(), value.end()},
                             std::string{typeString}},
                 timestamp);
  }

 private:
  template <typename T>
  void AppendAction(T&& value, int64_t timestamp = 0) {
    std::scoped_lock lock{m_backend.m_mutex};
    if (m_removed) {
      return;
    }
    m_last = m_backend.m_actions.size();
    m_backend.m_actions.emplace_back(m_path, std::forward<T>(value), timestamp);
  }

  std::string m_path;
  MockTelemetryBackend& m_backend;
  int m_last = -1;
  bool m_removed = false;
};

MockTelemetryBackend::MockTelemetryBackend() = default;

MockTelemetryBackend::~MockTelemetryBackend() = default;

void MockTelemetryBackend::Clear() {
  std::scoped_lock lock{m_mutex};
  for (auto&& kv : m_entries) {
    kv.second->ClearLast();
  }
  m_actions.clear();
}

std::shared_ptr<TelemetryEntry> MockTelemetryBackend::GetEntry(
    std::string_view path) {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    return it->second;
  }

  auto entry = std::make_shared<Entry>(path, *this);
  m_entries.try_emplace(path, entry);
  return entry;
}

void MockTelemetryBackend::RemoveEntry(std::string_view path) {
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    it->second->MarkRemoved();
    m_entries.erase(it);
  }
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
  std::string pathBuf;
  path = detail::NormalizeName(path, pathBuf);
  auto it = m_entries.find(path);
  if (it == m_entries.end()) {
    return nullptr;
  }
  int n = it->second->GetLast();
  if (n == -1) {
    return nullptr;
  }
  return &m_actions[n];
}
