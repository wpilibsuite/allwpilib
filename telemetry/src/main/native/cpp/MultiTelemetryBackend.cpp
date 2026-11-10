// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/MultiTelemetryBackend.hpp"

#include <atomic>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "wpi/telemetry/TelemetryEntry.hpp"

using namespace wpi;
using namespace wpi::telemetry;

namespace {
void ValidateBackends(
    std::span<const std::shared_ptr<TelemetryBackend>> backends) {
  for (const auto& backend : backends) {
    if (!backend) {
      throw std::invalid_argument{
          "MultiTelemetryBackend child backend cannot be null"};
    }
  }
}
}  // namespace

class MultiTelemetryBackend::Entry : public TelemetryEntry {
 public:
  Entry(std::string_view path,
        const std::vector<std::shared_ptr<TelemetryBackend>>& backends) {
    m_entries.reserve(backends.size());
    for (const auto& backend : backends) {
      m_entries.emplace_back(backend->GetEntry(path));
    }
  }

  void MarkRemoved() { m_removed.store(true); }

  bool IsDiscard() const override {
    if (m_removed.load()) {
      return true;
    }
    for (const auto& entry : m_entries) {
      if (!entry->IsDiscard()) {
        return false;
      }
    }
    return true;
  }

  void KeepDuplicates() override {
    ForEachEntry([](TelemetryEntry& entry) { entry.KeepDuplicates(); });
  }

  void SetProperty(std::string_view key, std::string_view value) override {
    ForEachEntry([&](TelemetryEntry& entry) { entry.SetProperty(key, value); });
  }

  void LogBoolean(bool value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogBoolean(value, timestamp); });
  }

  void LogInt8(int8_t value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogInt8(value, timestamp); });
  }

  void LogInt16(int16_t value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogInt16(value, timestamp); });
  }

  void LogInt32(int32_t value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogInt32(value, timestamp); });
  }

  void LogInt64(int64_t value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogInt64(value, timestamp); });
  }

  void LogFloat(float value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogFloat(value, timestamp); });
  }

  void LogDouble(double value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogDouble(value, timestamp); });
  }

  void LogString(std::string_view value, std::string_view typeString,
                 int64_t timestamp) override {
    ForEachEntry([=](TelemetryEntry& entry) {
      entry.LogString(value, typeString, timestamp);
    });
  }

  void LogBooleanArray(std::span<const bool> value,
                       int64_t timestamp) override {
    ForEachEntry([=](TelemetryEntry& entry) {
      entry.LogBooleanArray(value, timestamp);
    });
  }

  void LogBooleanArray(std::span<const int> value, int64_t timestamp) override {
    ForEachEntry([=](TelemetryEntry& entry) {
      entry.LogBooleanArray(value, timestamp);
    });
  }

  void LogInt16Array(std::span<const int16_t> value,
                     int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogInt16Array(value, timestamp); });
  }

  void LogInt32Array(std::span<const int32_t> value,
                     int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogInt32Array(value, timestamp); });
  }

  void LogInt64Array(std::span<const int64_t> value,
                     int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogInt64Array(value, timestamp); });
  }

  void LogFloatArray(std::span<const float> value, int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogFloatArray(value, timestamp); });
  }

  void LogDoubleArray(std::span<const double> value,
                      int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogDoubleArray(value, timestamp); });
  }

  void LogStringArray(std::span<const std::string> value,
                      int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogStringArray(value, timestamp); });
  }

  void LogStringArray(std::span<const std::string_view> value,
                      int64_t timestamp) override {
    ForEachEntry(
        [=](TelemetryEntry& entry) { entry.LogStringArray(value, timestamp); });
  }

  void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
              int64_t timestamp) override {
    ForEachEntry([=](TelemetryEntry& entry) {
      entry.LogRaw(value, typeString, timestamp);
    });
  }

 private:
  template <typename F>
  void ForEachEntry(F&& func) {
    if (m_removed.load()) {
      return;
    }
    for (const auto& entry : m_entries) {
      if (m_removed.load()) {
        return;
      }
      func(*entry);
    }
  }

  std::vector<std::shared_ptr<TelemetryEntry>> m_entries;
  std::atomic_bool m_removed{false};
};

MultiTelemetryBackend::MultiTelemetryBackend() = default;

MultiTelemetryBackend::MultiTelemetryBackend(
    std::span<const std::shared_ptr<TelemetryBackend>> backends)
    : m_backends{backends.begin(), backends.end()} {
  ValidateBackends(
      std::span<const std::shared_ptr<TelemetryBackend>>{m_backends});
}

MultiTelemetryBackend::MultiTelemetryBackend(
    std::vector<std::shared_ptr<TelemetryBackend>> backends)
    : m_backends{std::move(backends)} {
  ValidateBackends(
      std::span<const std::shared_ptr<TelemetryBackend>>{m_backends});
}

MultiTelemetryBackend::MultiTelemetryBackend(
    std::initializer_list<std::shared_ptr<TelemetryBackend>> backends)
    : MultiTelemetryBackend{std::span<const std::shared_ptr<TelemetryBackend>>{
          backends.begin(), backends.size()}} {}

MultiTelemetryBackend::~MultiTelemetryBackend() = default;

std::shared_ptr<TelemetryEntry> MultiTelemetryBackend::GetEntry(
    std::string_view path) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    return it->second;
  }

  auto entry = std::make_shared<Entry>(path, m_backends);
  m_entries.try_emplace(path, entry);
  return entry;
}

void MultiTelemetryBackend::RemoveEntry(std::string_view path) {
  {
    std::scoped_lock lock{m_mutex};
    auto it = m_entries.find(path);
    if (it != m_entries.end()) {
      auto entry = std::move(it->second);
      m_entries.erase(it);
      entry->MarkRemoved();
    }
  }
  for (const auto& backend : m_backends) {
    backend->RemoveEntry(path);
  }
}

bool MultiTelemetryBackend::HasSchema(std::string_view schemaName) const {
  for (const auto& backend : m_backends) {
    if (!backend->HasSchema(schemaName)) {
      return false;
    }
  }
  return true;
}

void MultiTelemetryBackend::AddSchema(std::string_view schemaName,
                                      std::string_view type,
                                      std::span<const uint8_t> schema) {
  for (const auto& backend : m_backends) {
    backend->AddSchema(schemaName, type, schema);
  }
}

void MultiTelemetryBackend::AddSchema(std::string_view schemaName,
                                      std::string_view type,
                                      std::string_view schema) {
  for (const auto& backend : m_backends) {
    backend->AddSchema(schemaName, type, schema);
  }
}
