// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/DiscardTelemetryBackend.hpp"

#include <memory>
#include <string>

#include "wpi/telemetry/TelemetryEntry.hpp"

using namespace wpi;
using namespace wpi::telemetry;

namespace {
class Entry : public TelemetryEntry {
 public:
  bool IsDiscard() const override { return true; }

  void KeepDuplicates() override {}

  void SetProperty(std::string_view key, std::string_view value) override {}

  void LogBoolean(bool value, int64_t timestamp) override {}

  void LogInt64(int64_t value, int64_t timestamp) override {}

  void LogFloat(float value, int64_t timestamp) override {}

  void LogDouble(double value, int64_t timestamp) override {}

  void LogString(std::string_view value, std::string_view typeString,
                 int64_t timestamp) override {}

  void LogBooleanArray(std::span<const bool> value,
                       int64_t timestamp) override {}

  void LogBooleanArray(std::span<const int> value, int64_t timestamp) override {
  }

  void LogInt16Array(std::span<const int16_t> value,
                     int64_t timestamp) override {}

  void LogInt32Array(std::span<const int32_t> value,
                     int64_t timestamp) override {}

  void LogInt64Array(std::span<const int64_t> value,
                     int64_t timestamp) override {}

  void LogFloatArray(std::span<const float> value, int64_t timestamp) override {
  }

  void LogDoubleArray(std::span<const double> value,
                      int64_t timestamp) override {}

  void LogStringArray(std::span<const std::string> value,
                      int64_t timestamp) override {}

  void LogStringArray(std::span<const std::string_view> value,
                      int64_t timestamp) override {}

  void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
              int64_t timestamp) override {}
};
}  // namespace

std::shared_ptr<TelemetryEntry> DiscardTelemetryBackend::GetEntry(
    std::string_view path) {
  static auto entry = std::make_shared<Entry>();
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
