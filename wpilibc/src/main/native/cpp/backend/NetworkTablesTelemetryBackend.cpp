// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/NetworkTablesTelemetryBackend.hpp"

#include <atomic>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi::backend;

class NetworkTablesTelemetryBackend::Entry
    : public wpi::telemetry::TelemetryEntry {
 public:
  Entry(wpi::nt::NetworkTableInstance inst, std::string_view prefix,
        std::string_view path)
      : m_inst{inst}, m_path{std::format("{}{}", prefix, path)} {}

  void MarkRemoved() {
    std::scoped_lock lock{m_mutex};
    m_pub = {};
    m_removed.store(true);
  }

  bool IsDiscard() const override { return m_removed.load(); }

  void KeepDuplicates() override {
    std::scoped_lock lock{m_mutex};
    if (m_removed.load()) {
      return;
    }
    if (m_keepDuplicates) {
      return;
    }
    m_keepDuplicates = true;
    if (m_pub) {
      auto newPub = Publish(m_typeString);
      if (newPub) {
        auto oldPub = std::move(m_pub);
        m_pub = std::move(newPub);
      }
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
    if (m_pub) {
      m_pub.GetTopic().SetProperties(m_properties);
    }
  }

  void LogBoolean(bool value, int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("boolean");
      }
      typeMismatch = !m_pub.SetBoolean(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogInt64(int64_t value, int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("int");
      }
      typeMismatch = !m_pub.SetInteger(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogFloat(float value, int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("float");
      }
      typeMismatch = !m_pub.SetFloat(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogDouble(double value, int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("double");
      }
      typeMismatch = !m_pub.SetDouble(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogString(std::string_view value, std::string_view typeString,
                 int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish(typeString);
      }
      typeMismatch =
          m_typeString != typeString || !m_pub.SetString(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogBooleanArray(std::span<const bool> value,
                       int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("boolean[]");
      }
      typeMismatch = !m_pub.SetBooleanArray(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogBooleanArray(std::span<const int> value, int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("boolean[]");
      }
      typeMismatch = !m_pub.SetBooleanArray(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
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
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("int[]");
      }
      typeMismatch = !m_pub.SetIntegerArray(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogFloatArray(std::span<const float> value, int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("float[]");
      }
      typeMismatch = !m_pub.SetFloatArray(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogDoubleArray(std::span<const double> value,
                      int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("double[]");
      }
      typeMismatch = !m_pub.SetDoubleArray(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogStringArray(std::span<const std::string> value,
                      int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish("string[]");
      }
      typeMismatch = !m_pub.SetStringArray(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogStringArray(std::span<const std::string_view> value,
                      int64_t timestamp) override {
    std::vector<std::string> arr;
    arr.reserve(value.size());
    for (auto&& val : value) {
      arr.emplace_back(val);
    }
    LogStringArray(arr, timestamp);
  }

  void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
              int64_t timestamp) override {
    bool typeMismatch = false;
    {
      std::scoped_lock lock{m_mutex};
      if (m_removed.load()) {
        return;
      }
      if (!m_pub) {
        m_pub = Publish(typeString);
      }
      typeMismatch =
          m_typeString != typeString || !m_pub.SetRaw(value, timestamp);
    }
    if (typeMismatch) {
      wpi::telemetry::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

 private:
  wpi::nt::GenericPublisher Publish(std::string_view typeString) {
    m_typeString = typeString;
    return m_inst.GetTopic(m_path).GenericPublishEx(
        m_typeString, m_properties, {.keepDuplicates = m_keepDuplicates});
  }

  wpi::nt::NetworkTableInstance m_inst;
  std::string m_path;
  wpi::util::mutex m_mutex;
  wpi::nt::GenericPublisher m_pub;
  std::atomic_bool m_removed{false};
  std::string m_typeString;
  std::atomic_bool m_keepDuplicates{false};
  wpi::util::json m_properties = wpi::util::json::object();
};

NetworkTablesTelemetryBackend::NetworkTablesTelemetryBackend(
    wpi::nt::NetworkTableInstance inst, std::string_view prefix)
    : m_inst{inst}, m_prefix{prefix} {}

NetworkTablesTelemetryBackend::~NetworkTablesTelemetryBackend() = default;

std::shared_ptr<wpi::telemetry::TelemetryEntry>
NetworkTablesTelemetryBackend::GetEntry(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    return it->second;
  }

  auto entry = std::make_shared<Entry>(m_inst, m_prefix, path);
  m_entries.try_emplace(path, entry);
  return entry;
}

void NetworkTablesTelemetryBackend::RemoveEntry(std::string_view path) {
  std::scoped_lock lock{m_mutex};
  auto it = m_entries.find(path);
  if (it != m_entries.end()) {
    auto entry = std::move(it->second);
    m_entries.erase(it);
    entry->MarkRemoved();
  }
}

bool NetworkTablesTelemetryBackend::HasSchema(
    std::string_view schemaName) const {
  return m_inst.HasSchema(schemaName);
}

void NetworkTablesTelemetryBackend::AddSchema(std::string_view schemaName,
                                              std::string_view type,
                                              std::span<const uint8_t> schema) {
  m_inst.AddSchema(schemaName, type, schema);
}

void NetworkTablesTelemetryBackend::AddSchema(std::string_view schemaName,
                                              std::string_view type,
                                              std::string_view schema) {
  m_inst.AddSchema(schemaName, type, schema);
}
