// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/NetworkTablesTelemetryBackend.hpp"

#include <atomic>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>

#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi::nt;

class NetworkTablesTelemetryBackend::Entry : public wpi::TelemetryEntry {
 public:
  Entry(NetworkTableInstance inst, std::string_view prefix,
        std::string_view path)
      : m_inst{inst}, m_path{fmt::format("{}{}", prefix, path)} {}

  void KeepDuplicates() override {
    m_keepDuplicates = true;
    // TODO: update publisher
  }

  void SetProperty(std::string_view key, std::string_view value) override {
    std::scoped_lock lock{m_mutex};
    auto it = m_properties.find(key);
    if (it == m_properties.end()) {
      m_properties.emplace(key, value);
    } else {
      auto& curVal = it.value().get_ref<std::string&>();
      if (curVal == value) {
        return;
      }
      curVal = value;
    }
    if (m_pub) {
      m_pub.GetTopic().SetProperties(m_properties);
    }
  }

  void LogBoolean(bool value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "boolean", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetBoolean(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogInt64(int64_t value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "int", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetInteger(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogFloat(float value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "float", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetFloat(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogDouble(double value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "double", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetDouble(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogString(std::string_view value, std::string_view typeString) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_typeString = typeString;
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          typeString, m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (m_typeString != typeString || !m_pub.SetString(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogBooleanArray(std::span<const bool> value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "boolean[]", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetBooleanArray(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogBooleanArray(std::span<const int> value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "boolean[]", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetBooleanArray(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogInt16Array(std::span<const int16_t> value) override {
    // TODO
  }

  void LogInt32Array(std::span<const int32_t> value) override {
    // TODO
  }

  void LogInt64Array(std::span<const int64_t> value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "int[]", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetIntegerArray(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogFloatArray(std::span<const float> value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "float[]", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetFloatArray(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogDoubleArray(std::span<const double> value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "double[]", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetDoubleArray(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogStringArray(std::span<const std::string> value) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          "string[]", m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (!m_pub.SetStringArray(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

  void LogStringArray(std::span<const std::string_view> value) override {
    // TODO
  }

  void LogRaw(std::span<const uint8_t> value,
              std::string_view typeString) override {
    std::scoped_lock lock{m_mutex};
    if (!m_pub) {
      m_typeString = typeString;
      m_pub = m_inst.GetTopic(m_path).GenericPublishEx(
          typeString, m_properties, {.keepDuplicates = m_keepDuplicates});
    }
    if (m_typeString != typeString || !m_pub.SetRaw(value)) {
      wpi::TelemetryRegistry::ReportWarning(m_path, "type mismatch");
    }
  }

 private:
  NetworkTableInstance m_inst;
  std::string m_path;
  wpi::util::mutex m_mutex;
  GenericPublisher m_pub;
  std::string m_typeString;
  std::atomic_bool m_keepDuplicates{false};
  wpi::util::json m_properties = wpi::util::json::object();
};

NetworkTablesTelemetryBackend::NetworkTablesTelemetryBackend(
    NetworkTableInstance inst, std::string_view prefix)
    : m_inst{inst}, m_prefix{prefix} {}

NetworkTablesTelemetryBackend::~NetworkTablesTelemetryBackend() = default;

wpi::TelemetryEntry& NetworkTablesTelemetryBackend::GetEntry(
    std::string_view path) {
  std::scoped_lock lock{m_mutex};
  return m_entries.try_emplace(path, m_inst, m_prefix, path).first->second;
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
