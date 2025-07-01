// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/DiscardTelemetryBackend.hpp"

#include <string>

#include "wpi/telemetry/TelemetryEntry.hpp"

using namespace wpi;

namespace {
class Entry : public TelemetryEntry {
 public:
  void KeepDuplicates() override {}

  void SetProperty(std::string_view key, std::string_view value) override {}

  void LogBoolean(bool value) override {}

  void LogInt64(int64_t value) override {}

  void LogFloat(float value) override {}

  void LogDouble(double value) override {}

  void LogString(std::string_view value, std::string_view typeString) override {
  }

  void LogBooleanArray(std::span<const bool> value) override {}

  void LogBooleanArray(std::span<const int> value) override {}

  void LogInt16Array(std::span<const int16_t> value) override {}

  void LogInt32Array(std::span<const int32_t> value) override {}

  void LogInt64Array(std::span<const int64_t> value) override {}

  void LogFloatArray(std::span<const float> value) override {}

  void LogDoubleArray(std::span<const double> value) override {}

  void LogStringArray(std::span<const std::string> value) override {}

  void LogStringArray(std::span<const std::string_view> value) override {}

  void LogRaw(std::span<const uint8_t> value,
              std::string_view typeString) override {}
};
}  // namespace

TelemetryEntry& DiscardTelemetryBackend::GetEntry(std::string_view path) {
  static Entry entry;
  return entry;
}

bool DiscardTelemetryBackend::HasSchema(std::string_view schemaName) const {
  return true;
}

void DiscardTelemetryBackend::AddSchema(std::string_view schemaName,
                                        std::string_view type,
                                        std::span<const uint8_t> schema) {}

void DiscardTelemetryBackend::AddSchema(std::string_view schemaName,
                                        std::string_view type,
                                        std::string_view schema) {}
