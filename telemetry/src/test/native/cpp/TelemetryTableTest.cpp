// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryTable.hpp"  // NOLINT(build/include_order)

#include <stdint.h>

#include <array>
#include <atomic>
#include <format>
#include <future>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/telemetry/DiscardTelemetryBackend.hpp"
#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/MultiTelemetryBackend.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/StringMap.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace telemetrytest {
struct TestStructADL {
  double x = 0;
  double y = 0;
};

void LogTo(wpi::telemetry::TelemetryTable& table, const TestStructADL& value) {
  table.Log("x", value.x);
  table.Log("y", value.y);
}

struct TestStructADLType {
  double x = 0;
  double y = 0;
};

std::string_view GetTelemetryTypeName(const TestStructADLType&) {
  return "TestStructADLType";
}

void LogTo(wpi::telemetry::TelemetryTable& table,
           const TestStructADLType& value) {
  table.Log("x", value.x);
  table.Log("y", value.y);
}

struct TestStructLoggable : public wpi::telemetry::TelemetryLoggable {
  double x = 0;
  double y = 0;

  TestStructLoggable(double x_, double y_) : x{x_}, y{y_} {}

  void LogTo(wpi::telemetry::TelemetryTable& table) const override {
    table.Log("x", x);
    table.Log("y", y);
  }
};

struct TestStructLoggableType : public TestStructLoggable {
  TestStructLoggableType(double x_, double y_) : TestStructLoggable{x_, y_} {}

  std::string_view GetTelemetryType() const override {
    return "TestStructLoggableType";
  }
};

struct BlockingTypedLoggable : public wpi::telemetry::TelemetryLoggable {
  BlockingTypedLoggable(std::promise<void>* entered,
                        std::shared_future<void> release)
      : enteredLogTo{entered}, releaseLogTo{std::move(release)} {}

  std::promise<void>* enteredLogTo;
  std::shared_future<void> releaseLogTo;

  std::string_view GetTelemetryType() const override {
    return "BlockingTypedLoggable";
  }

  void LogTo(wpi::telemetry::TelemetryTable& table) const override {
    enteredLogTo->set_value();
    releaseLogTo.wait();
    table.Log("x", 1.0);
  }
};

struct ValueStyle {
  double value = 0;
};

void LogValueTo(wpi::telemetry::TelemetryTable& table, std::string_view name,
                const ValueStyle& value) {
  table.Log(name, value.value);
}

struct StructPoint {
  double x = 0;
  int32_t y = 0;
};

class CountingSchemaBackend : public wpi::telemetry::MockTelemetryBackend {
 public:
  int GetSchemaAddCount() const { return m_schemaAdds.load(); }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::span<const uint8_t> schema) override {
    ++m_schemaAdds;
    wpi::telemetry::MockTelemetryBackend::AddSchema(schemaName, type, schema);
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::string_view schema) override {
    ++m_schemaAdds;
    wpi::telemetry::MockTelemetryBackend::AddSchema(schemaName, type, schema);
  }

 private:
  std::atomic_int m_schemaAdds{0};
};

class RegisteringTelemetryBackend
    : public wpi::telemetry::MockTelemetryBackend {
 public:
  explicit RegisteringTelemetryBackend(
      std::shared_ptr<wpi::telemetry::MockTelemetryBackend> replacement)
      : m_replacement{std::move(replacement)} {}

  std::shared_ptr<wpi::telemetry::TelemetryEntry> GetEntry(
      std::string_view path) override {
    if (path == "/rerouted/.type" && !m_registered.exchange(true)) {
      wpi::telemetry::TelemetryRegistry::RegisterBackend("", m_replacement);
    }
    return wpi::telemetry::MockTelemetryBackend::GetEntry(path);
  }

 private:
  std::shared_ptr<wpi::telemetry::MockTelemetryBackend> m_replacement;
  std::atomic_bool m_registered{false};
};

struct Formattable {
  int value = 0;
};

struct ThrowingFormattable {};

struct ThrowingLoggable : public wpi::telemetry::TelemetryLoggable {
  void LogTo(wpi::telemetry::TelemetryTable& table) const override {
    (void)table;
    throw std::logic_error{"LogTo should not run"};
  }
};

struct RobotSpeed : public wpi::telemetry::TelemetryLoggable {
  explicit RobotSpeed(double speed_) : speed{speed_} {}

  double speed = 0.0;

  void LogTo(wpi::telemetry::TelemetryTable& table) const override {
    table.Log("speed", speed);
  }
};

struct BlockingBackendState {
  BlockingBackendState() : releaseFuture{releaseLog.get_future().share()} {}

  std::promise<void> enteredLog;
  std::promise<void> releaseLog;
  std::shared_future<void> releaseFuture;
  std::atomic_bool block{false};
  std::atomic_bool entered{false};
  std::atomic_bool destroyed{false};
  std::atomic_int logs{0};
};

class BlockingTelemetryBackend : public wpi::telemetry::TelemetryBackend {
 public:
  explicit BlockingTelemetryBackend(std::shared_ptr<BlockingBackendState> state)
      : m_state{std::move(state)}, m_entry{std::make_shared<Entry>(*m_state)} {}

  ~BlockingTelemetryBackend() override { m_state->destroyed.store(true); }

  std::shared_ptr<wpi::telemetry::TelemetryEntry> GetEntry(
      std::string_view path) override {
    (void)path;
    return m_entry;
  }

  bool HasSchema(std::string_view schemaName) const override {
    (void)schemaName;
    return false;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::span<const uint8_t> schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::string_view schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

 private:
  class Entry : public wpi::telemetry::TelemetryEntry {
   public:
    explicit Entry(BlockingBackendState& state) : m_state{state} {}

    void KeepDuplicates() override {}

    void SetProperty(std::string_view key, std::string_view value) override {
      (void)key;
      (void)value;
    }

    void LogBoolean(bool value, int64_t) override { (void)value; }

    void LogInt64(int64_t value, int64_t) override { (void)value; }

    void LogFloat(float value, int64_t timestamp) override {
      LogDouble(value, timestamp);
    }

    void LogDouble(double value, int64_t) override {
      (void)value;
      ++m_state.logs;
      if (!m_state.block.load()) {
        return;
      }
      if (!m_state.entered.exchange(true)) {
        m_state.enteredLog.set_value();
      }
      m_state.releaseFuture.wait();
    }

    void LogString(std::string_view value, std::string_view typeString,
                   int64_t) override {
      (void)value;
      (void)typeString;
    }

    void LogBooleanArray(std::span<const bool> value, int64_t) override {
      (void)value;
    }

    void LogBooleanArray(std::span<const int> value, int64_t) override {
      (void)value;
    }

    void LogInt16Array(std::span<const int16_t> value, int64_t) override {
      (void)value;
    }

    void LogInt32Array(std::span<const int32_t> value, int64_t) override {
      (void)value;
    }

    void LogInt64Array(std::span<const int64_t> value, int64_t) override {
      (void)value;
    }

    void LogFloatArray(std::span<const float> value, int64_t) override {
      (void)value;
    }

    void LogDoubleArray(std::span<const double> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string_view> value,
                        int64_t) override {
      (void)value;
    }

    void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
                int64_t) override {
      (void)value;
      (void)typeString;
    }

   private:
    BlockingBackendState& m_state;
  };

  std::shared_ptr<BlockingBackendState> m_state;
  std::shared_ptr<Entry> m_entry;
};

class BlockingIsDiscardBackend : public wpi::telemetry::TelemetryBackend {
 public:
  BlockingIsDiscardBackend(std::promise<void>* entered,
                           std::shared_future<void> release, bool discard)
      : m_entry{std::make_shared<Entry>(entered, std::move(release), discard)} {
  }

  std::shared_ptr<wpi::telemetry::TelemetryEntry> GetEntry(
      std::string_view path) override {
    (void)path;
    return m_entry;
  }

  bool HasSchema(std::string_view schemaName) const override {
    (void)schemaName;
    return false;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::span<const uint8_t> schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::string_view schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

 private:
  class Entry : public wpi::telemetry::TelemetryEntry {
   public:
    Entry(std::promise<void>* entered, std::shared_future<void> release,
          bool discard)
        : m_entered{entered},
          m_release{std::move(release)},
          m_discard{discard} {}

    bool IsDiscard() const override {
      if (!m_enteredCalled.exchange(true)) {
        m_entered->set_value();
      }
      m_release.wait();
      return m_discard;
    }

    void KeepDuplicates() override {}

    void SetProperty(std::string_view key, std::string_view value) override {
      (void)key;
      (void)value;
    }

    void LogBoolean(bool value, int64_t) override { (void)value; }

    void LogInt64(int64_t value, int64_t) override { (void)value; }

    void LogFloat(float value, int64_t) override { (void)value; }

    void LogDouble(double value, int64_t) override { (void)value; }

    void LogString(std::string_view value, std::string_view typeString,
                   int64_t) override {
      (void)value;
      (void)typeString;
    }

    void LogBooleanArray(std::span<const bool> value, int64_t) override {
      (void)value;
    }

    void LogBooleanArray(std::span<const int> value, int64_t) override {
      (void)value;
    }

    void LogInt16Array(std::span<const int16_t> value, int64_t) override {
      (void)value;
    }

    void LogInt32Array(std::span<const int32_t> value, int64_t) override {
      (void)value;
    }

    void LogInt64Array(std::span<const int64_t> value, int64_t) override {
      (void)value;
    }

    void LogFloatArray(std::span<const float> value, int64_t) override {
      (void)value;
    }

    void LogDoubleArray(std::span<const double> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string_view> value,
                        int64_t) override {
      (void)value;
    }

    void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
                int64_t) override {
      (void)value;
      (void)typeString;
    }

   private:
    std::promise<void>* m_entered;
    std::shared_future<void> m_release;
    bool m_discard;
    mutable std::atomic_bool m_enteredCalled{false};
  };

  std::shared_ptr<Entry> m_entry;
};

class GenerationTelemetryBackend : public wpi::telemetry::TelemetryBackend {
 public:
  std::shared_ptr<wpi::telemetry::TelemetryEntry> GetEntry(
      std::string_view path) override {
    auto it = m_entries.find(path);
    if (it != m_entries.end()) {
      return it->second;
    }

    auto entry = std::make_shared<Entry>(*this, ++m_nextGeneration);
    m_entries.try_emplace(path, entry);
    return entry;
  }

  void RemoveEntry(std::string_view path) override {
    auto it = m_entries.find(path);
    if (it != m_entries.end()) {
      m_entries.erase(it);
    }
  }

  bool HasSchema(std::string_view schemaName) const override {
    (void)schemaName;
    return false;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::span<const uint8_t> schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::string_view schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

  const std::vector<int>& GetLogGenerations() const { return m_logGenerations; }

 private:
  class Entry : public wpi::telemetry::TelemetryEntry {
   public:
    Entry(GenerationTelemetryBackend& backend, int generation)
        : m_backend{backend}, m_generation{generation} {}

    void KeepDuplicates() override {}

    void SetProperty(std::string_view key, std::string_view value) override {
      (void)key;
      (void)value;
    }

    void LogBoolean(bool value, int64_t) override { (void)value; }

    void LogInt64(int64_t value, int64_t) override { (void)value; }

    void LogFloat(float value, int64_t) override { (void)value; }

    void LogDouble(double value, int64_t) override {
      (void)value;
      m_backend.m_logGenerations.emplace_back(m_generation);
    }

    void LogString(std::string_view value, std::string_view typeString,
                   int64_t) override {
      (void)value;
      (void)typeString;
    }

    void LogBooleanArray(std::span<const bool> value, int64_t) override {
      (void)value;
    }

    void LogBooleanArray(std::span<const int> value, int64_t) override {
      (void)value;
    }

    void LogInt16Array(std::span<const int16_t> value, int64_t) override {
      (void)value;
    }

    void LogInt32Array(std::span<const int32_t> value, int64_t) override {
      (void)value;
    }

    void LogInt64Array(std::span<const int64_t> value, int64_t) override {
      (void)value;
    }

    void LogFloatArray(std::span<const float> value, int64_t) override {
      (void)value;
    }

    void LogDoubleArray(std::span<const double> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string_view> value,
                        int64_t) override {
      (void)value;
    }

    void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
                int64_t) override {
      (void)value;
      (void)typeString;
    }

   private:
    GenerationTelemetryBackend& m_backend;
    int m_generation;
  };

  int m_nextGeneration = 0;
  wpi::util::StringMap<std::shared_ptr<Entry>> m_entries;
  std::vector<int> m_logGenerations;
};

class ClosingTelemetryBackend : public wpi::telemetry::TelemetryBackend {
 public:
  std::shared_ptr<wpi::telemetry::TelemetryEntry> GetEntry(
      std::string_view path) override {
    auto it = m_entries.find(path);
    if (it != m_entries.end()) {
      return it->second;
    }

    auto entry = std::make_shared<Entry>(*this);
    m_entries.try_emplace(path, entry);
    return entry;
  }

  void RemoveEntry(std::string_view path) override {
    auto it = m_entries.find(path);
    if (it != m_entries.end()) {
      it->second->MarkRemoved();
      m_entries.erase(it);
      ++m_removes;
    }
  }

  bool HasSchema(std::string_view schemaName) const override {
    (void)schemaName;
    return false;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::span<const uint8_t> schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

  void AddSchema(std::string_view schemaName, std::string_view type,
                 std::string_view schema) override {
    (void)schemaName;
    (void)type;
    (void)schema;
  }

  int GetLogs() const { return m_logs.load(); }

  int GetRemoves() const { return m_removes.load(); }

 private:
  class Entry : public wpi::telemetry::TelemetryEntry {
   public:
    explicit Entry(ClosingTelemetryBackend& backend) : m_backend{backend} {}

    void MarkRemoved() { m_removed.store(true); }

    bool IsDiscard() const override { return m_removed.load(); }

    void KeepDuplicates() override {}

    void SetProperty(std::string_view key, std::string_view value) override {
      (void)key;
      (void)value;
    }

    void LogBoolean(bool value, int64_t) override { (void)value; }

    void LogInt64(int64_t value, int64_t) override { (void)value; }

    void LogFloat(float value, int64_t timestamp) override {
      LogDouble(value, timestamp);
    }

    void LogDouble(double value, int64_t) override {
      (void)value;
      if (!m_removed.load()) {
        ++m_backend.m_logs;
      }
    }

    void LogString(std::string_view value, std::string_view typeString,
                   int64_t) override {
      (void)value;
      (void)typeString;
    }

    void LogBooleanArray(std::span<const bool> value, int64_t) override {
      (void)value;
    }

    void LogBooleanArray(std::span<const int> value, int64_t) override {
      (void)value;
    }

    void LogInt16Array(std::span<const int16_t> value, int64_t) override {
      (void)value;
    }

    void LogInt32Array(std::span<const int32_t> value, int64_t) override {
      (void)value;
    }

    void LogInt64Array(std::span<const int64_t> value, int64_t) override {
      (void)value;
    }

    void LogFloatArray(std::span<const float> value, int64_t) override {
      (void)value;
    }

    void LogDoubleArray(std::span<const double> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string> value, int64_t) override {
      (void)value;
    }

    void LogStringArray(std::span<const std::string_view> value,
                        int64_t) override {
      (void)value;
    }

    void LogRaw(std::span<const uint8_t> value, std::string_view typeString,
                int64_t) override {
      (void)value;
      (void)typeString;
    }

   private:
    ClosingTelemetryBackend& m_backend;
    std::atomic_bool m_removed{false};
  };

  wpi::util::StringMap<std::shared_ptr<Entry>> m_entries;
  std::atomic_int m_logs{0};
  std::atomic_int m_removes{0};
};
}  // namespace telemetrytest

template <>
struct wpi::util::Struct<telemetrytest::StructPoint> {
  static constexpr std::string_view GetTypeName() {
    return "telemetrytest.StructPoint";
  }
  static constexpr size_t GetSize() { return 12; }
  static constexpr std::string_view GetSchema() { return "double x; int32 y"; }

  static telemetrytest::StructPoint Unpack(std::span<const uint8_t> data) {
    return {wpi::util::UnpackStruct<double, 0>(data),
            wpi::util::UnpackStruct<int32_t, 8>(data)};
  }

  static void Pack(std::span<uint8_t> data,
                   const telemetrytest::StructPoint& value) {
    wpi::util::PackStruct<0>(data, value.x);
    wpi::util::PackStruct<8>(data, value.y);
  }
};

template <>
struct std::formatter<telemetrytest::Formattable> : std::formatter<int> {
  auto format(const telemetrytest::Formattable& value,
              format_context& ctx) const {
    return std::formatter<int>::format(value.value, ctx);
  }
};

template <>
struct std::formatter<telemetrytest::ThrowingFormattable>
    : std::formatter<int> {
  auto format(const telemetrytest::ThrowingFormattable& value,
              format_context& ctx) const {
    (void)value;
    throw std::logic_error{"format should not run"};
    return std::formatter<int>::format(0, ctx);
  }
};

struct TelemetryTableTest {
  TelemetryTableTest() {
    warnings.clear();
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::telemetry::TelemetryRegistry::SetReportWarning(
        [this](std::string_view path, std::string_view msg) {
          warnings.emplace_back(path, msg);
        });
    wpi::telemetry::TelemetryRegistry::RegisterBackend("", mock);
  }

  ~TelemetryTableTest() {
    wpi::telemetry::TelemetryRegistry::SetReportWarning(nullptr);
    wpi::telemetry::TelemetryRegistry::Reset();
  }

  template <typename T>
  T Last(std::string_view path) {
    auto value = mock->GetLastValue<T>(path);
    if (!value) {
      FAIL("No last value at "
           << path << " with requested variant type"
           << (mock->GetLastAction(path)
                   ? std::format("; actual variant index {}",
                                 mock->GetLastAction(path)->value.index())
                   : "; no action at that path"));
      return T{};
    }
    return *value;
  }

  std::shared_ptr<wpi::telemetry::MockTelemetryBackend> mock =
      std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  std::vector<std::pair<std::string, std::string>> warnings;
};

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest TablePathNormalizationAndCaching",
                 "[telemetry]") {
  auto& root = wpi::telemetry::TelemetryRegistry::GetTable("");
  auto& drive = wpi::telemetry::TelemetryRegistry::GetTable("drive");
  REQUIRE(root.GetPath() == "/");
  REQUIRE(drive.GetPath() == "/drive/");
  REQUIRE(&drive == &wpi::telemetry::TelemetryRegistry::GetTable("/drive/"));
  REQUIRE(&drive == &wpi::telemetry::GetTable("drive"));
  REQUIRE(&drive.GetTable("left") ==
          &wpi::telemetry::TelemetryRegistry::GetTable("/drive/left/"));
  REQUIRE(&drive.GetTable("left") == &drive.GetTable("/left"));
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest TableEntryPathsAreNormalized",
                 "[telemetry]") {
  wpi::telemetry::Log("/value", 1.0);
  wpi::telemetry::Log("value", 2.0);

  auto& drive = wpi::telemetry::GetTable("drive");
  drive.KeepDuplicates("/speed");
  drive.SetProperty("//speed", "unit", "\"m/s\"");
  drive.Log("/speed", 3.0);
  wpi::telemetry::GetTable("drive//").Log("speed", 4.0);

  REQUIRE(mock->GetLastValue<double>("/value") == 2.0);
  REQUIRE(mock->GetLastValue<double>("//value") == 2.0);
  bool sawKeepDuplicates = false;
  bool sawProperty = false;
  for (const auto& action : mock->GetActions()) {
    REQUIRE(action.path.find("//") == std::string::npos);
    if (std::holds_alternative<
            wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue>(
            action.value)) {
      REQUIRE(action.path == "/drive/speed");
      sawKeepDuplicates = true;
    }
    if (auto property =
            std::get_if<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
                &action.value)) {
      REQUIRE(action.path == "/drive/speed");
      REQUIRE(property->key == "unit");
      REQUIRE(property->value == "\"m/s\"");
      sawProperty = true;
    }
  }
  REQUIRE(sawKeepDuplicates);
  REQUIRE(sawProperty);
  REQUIRE(mock->GetLastValue<double>("/drive/speed") == 4.0);
  REQUIRE(mock->GetLastValue<double>("/drive//speed") == 4.0);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest MockBackendNormalizesPaths",
                 "[telemetry]") {
  auto entry = mock->GetEntry("drive//speed");
  entry->LogDouble(1.0, 0);

  REQUIRE_FALSE(entry->IsDiscard());
  REQUIRE(mock->GetLastValue<double>("/drive/speed") == 1.0);
  REQUIRE(mock->GetLastValue<double>("drive/speed") == 1.0);
  REQUIRE(mock->GetLastAction("//drive/speed")->path == "/drive/speed");

  mock->RemoveEntry("drive//speed");

  REQUIRE(entry->IsDiscard());
  REQUIRE_FALSE(mock->GetLastAction("/drive/speed"));

  entry->LogDouble(2.0, 0);

  REQUIRE_FALSE(mock->GetLastAction("/drive/speed"));

  auto newEntry = mock->GetEntry("/drive/speed");
  REQUIRE(newEntry != entry);
  REQUIRE_FALSE(newEntry->IsDiscard());

  newEntry->LogDouble(3.0, 0);

  REQUIRE(mock->GetLastValue<double>("/drive/speed") == 3.0);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest ReportWarningAllowsReentry",
                 "[telemetry]") {
  std::vector<std::pair<std::string, std::string>> reportedWarnings;
  bool nested = false;
  wpi::telemetry::TelemetryRegistry::SetReportWarning(
      [&](std::string_view path, std::string_view msg) {
        reportedWarnings.emplace_back(path, msg);
        auto reportWarning =
            wpi::telemetry::TelemetryRegistry::GetReportWarning();
        wpi::telemetry::TelemetryRegistry::SetReportWarning(reportWarning);
        if (!nested) {
          nested = true;
          wpi::telemetry::TelemetryRegistry::ReportWarning("/nested",
                                                           "nested warning");
        }
      });

  wpi::telemetry::TelemetryRegistry::ReportWarning("/outer", "outer warning");

  REQUIRE(reportedWarnings.size() == 2u);
  CHECK(reportedWarnings[0].first == "/outer");
  CHECK(reportedWarnings[0].second == "outer warning");
  CHECK(reportedWarnings[1].first == "/nested");
  CHECK(reportedWarnings[1].second == "nested warning");
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest TypeMismatchWarningAllowsTableReentry",
                 "[telemetry]") {
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/reentry/");
  REQUIRE(table.SetType("ExpectedType"));

  wpi::telemetry::TelemetryRegistry::SetReportWarning(
      [&](std::string_view path, std::string_view msg) {
        warnings.emplace_back(path, msg);
        CHECK(table.GetType() == "ExpectedType");
      });

  CHECK_FALSE(table.SetType("OtherType"));

  REQUIRE(warnings.size() == 1u);
  CHECK(warnings[0].first == "/reentry/");
  CHECK(warnings[0].second ==
        "table type mismatch, expected 'ExpectedType', got 'OtherType'");
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest FacadeMetadataAndPrimitiveLogging",
                 "[telemetry]") {
  wpi::telemetry::KeepDuplicates("loops");
  wpi::telemetry::SetProperty("loops", "unit", "\"count\"");
  wpi::telemetry::Log("enabled", true);
  wpi::telemetry::Log("short", int16_t{2});
  wpi::telemetry::Log("int", int32_t{3});
  wpi::telemetry::Log("long", int64_t{4});
  wpi::telemetry::Log("float", 5.0f);
  wpi::telemetry::Log("double", 6.0);
  wpi::telemetry::Log("state", std::string_view{"ready"});
  wpi::telemetry::Log("json", std::string_view{"{\"ok\":true}"},
                      std::string_view{"json"});

  REQUIRE(mock->GetActions()[0].path == "/loops");
  REQUIRE(std::holds_alternative<
          wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue>(
      mock->GetActions()[0].value));
  auto property =
      std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
          mock->GetActions()[1].value);
  REQUIRE(property.key == "unit");
  REQUIRE(property.value == "\"count\"");
  REQUIRE(Last<bool>("/enabled") == true);
  REQUIRE(Last<int16_t>("/short") == 2);
  REQUIRE(Last<int32_t>("/int") == 3);
  REQUIRE(Last<int64_t>("/long") == 4);
  REQUIRE(Last<float>("/float") == 5.0f);
  REQUIRE(Last<double>("/double") == 6.0);

  auto state =
      Last<wpi::telemetry::MockTelemetryBackend::LogStringValue>("/state");
  REQUIRE(state.value == "ready");
  REQUIRE(state.typeString == "string");
  auto json =
      Last<wpi::telemetry::MockTelemetryBackend::LogStringValue>("/json");
  REQUIRE(json.value == "{\"ok\":true}");
  REQUIRE(json.typeString == "json");
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest ArrayLoggingCopiesInputs", "[telemetry]") {
  std::array<bool, 2> bools{true, false};
  std::array<int16_t, 2> shorts{1, 2};
  std::array<int32_t, 2> ints{3, 4};
  std::array<int64_t, 2> longs{5, 6};
  std::array<float, 2> floats{7.0f, 8.0f};
  std::array<double, 2> doubles{9.0, 10.0};
  std::array<std::string_view, 2> strings{"a", "b"};
  std::array<uint8_t, 2> raw{11, 12};

  wpi::telemetry::Log("bools", std::span<const bool>{bools});
  wpi::telemetry::Log("shorts", std::span<const int16_t>{shorts});
  wpi::telemetry::Log("ints", std::span<const int32_t>{ints});
  wpi::telemetry::Log("longs", std::span<const int64_t>{longs});
  wpi::telemetry::Log("floats", std::span<const float>{floats});
  wpi::telemetry::Log("doubles", std::span<const double>{doubles});
  wpi::telemetry::Log("strings", std::span<const std::string_view>{strings});
  wpi::telemetry::Log("raw", std::span<const uint8_t>{raw},
                      std::string_view{"bytes"});
  wpi::telemetry::Log("initializerBools", {true, false});
  wpi::telemetry::Log("initializerShorts", {int16_t{1}, int16_t{2}});
  wpi::telemetry::Log("initializerInts", {int32_t{3}, int32_t{4}});
  wpi::telemetry::Log("initializerLongs", {int64_t{5}, int64_t{6}});
  wpi::telemetry::Log("initializerFloats", {7.0f, 8.0f});
  wpi::telemetry::Log("initializerDoubles", {9.0, 10.0});

  bools[0] = false;
  shorts[0] = 99;
  ints[0] = 99;
  longs[0] = 99;
  floats[0] = 99;
  doubles[0] = 99;
  strings[0] = "changed";
  raw[0] = 99;

  REQUIRE(
      Last<wpi::telemetry::MockTelemetryBackend::LogBooleanArrayValue>("/bools")
          .value == (std::vector<int>{1, 0}));
  REQUIRE(Last<std::vector<int16_t>>("/shorts") ==
          (std::vector<int16_t>{1, 2}));
  REQUIRE(Last<std::vector<int32_t>>("/ints") == (std::vector<int32_t>{3, 4}));
  REQUIRE(Last<std::vector<int64_t>>("/longs") == (std::vector<int64_t>{5, 6}));
  REQUIRE(Last<std::vector<float>>("/floats") ==
          (std::vector<float>{7.0f, 8.0f}));
  REQUIRE(Last<std::vector<double>>("/doubles") ==
          (std::vector<double>{9.0, 10.0}));
  REQUIRE(Last<std::vector<std::string>>("/strings") ==
          (std::vector<std::string>{"a", "b"}));
  auto rawValue =
      Last<wpi::telemetry::MockTelemetryBackend::LogRawValue>("/raw");
  REQUIRE(rawValue.value == (std::vector<uint8_t>{11, 12}));
  REQUIRE(rawValue.typeString == "bytes");
  REQUIRE(Last<wpi::telemetry::MockTelemetryBackend::LogBooleanArrayValue>(
              "/initializerBools")
              .value == (std::vector<int>{1, 0}));
  REQUIRE(Last<std::vector<int16_t>>("/initializerShorts") ==
          (std::vector<int16_t>{1, 2}));
  REQUIRE(Last<std::vector<int32_t>>("/initializerInts") ==
          (std::vector<int32_t>{3, 4}));
  REQUIRE(Last<std::vector<int64_t>>("/initializerLongs") ==
          (std::vector<int64_t>{5, 6}));
  REQUIRE(Last<std::vector<float>>("/initializerFloats") ==
          (std::vector<float>{7.0f, 8.0f}));
  REQUIRE(Last<std::vector<double>>("/initializerDoubles") ==
          (std::vector<double>{9.0, 10.0}));
}

TEST_CASE_METHOD(TelemetryTableTest, "TelemetryTableTest GenericDispatch",
                 "[telemetry]") {
  auto& table = wpi::telemetry::GetTable();
  table.Log("boolValue", true);
  table.Log("integralValue", int64_t{7});
  table.Log("floatingValue", 1.25);
  table.Log("stringValue", "hello");
  table.Log("boundedArray", std::array<int32_t, 2>{1, 2});
  table.Log("int8Array", std::array<int8_t, 2>{-1, 2});
  table.Log("unsignedArray", std::array<uint32_t, 2>{3, 4});
  table.Log("rawArray", std::array<uint8_t, 2>{5, 6});
  table.Log("formattable", telemetrytest::Formattable{42});
  std::array<telemetrytest::Formattable, 2> formattableArray{{{1}, {2}}};
  table.Log("formattableArray",
            std::span<const telemetrytest::Formattable>{formattableArray});

  REQUIRE(Last<bool>("/boolValue") == true);
  REQUIRE(Last<int64_t>("/integralValue") == 7);
  REQUIRE(Last<double>("/floatingValue") == 1.25);
  REQUIRE(
      Last<wpi::telemetry::MockTelemetryBackend::LogStringValue>("/stringValue")
          .value == "hello");
  REQUIRE(Last<std::vector<int32_t>>("/boundedArray") ==
          (std::vector<int32_t>{1, 2}));
  REQUIRE(Last<std::vector<int64_t>>("/int8Array") ==
          (std::vector<int64_t>{-1, 2}));
  REQUIRE(Last<std::vector<int64_t>>("/unsignedArray") ==
          (std::vector<int64_t>{3, 4}));
  REQUIRE(Last<wpi::telemetry::MockTelemetryBackend::LogRawValue>("/rawArray")
              .value == (std::vector<uint8_t>{5, 6}));
  REQUIRE(
      Last<wpi::telemetry::MockTelemetryBackend::LogStringValue>("/formattable")
          .value == "42");
  REQUIRE(Last<std::vector<std::string>>("/formattableArray") ==
          (std::vector<std::string>{"1", "2"}));
}

TEST_CASE_METHOD(TelemetryTableTest, "TelemetryTableTest LoggableAndADLObjects",
                 "[telemetry]") {
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/");
  table.Log("adl", telemetrytest::TestStructADL{1, 2});
  REQUIRE(Last<double>("/adl/x") == 1);
  REQUIRE(Last<double>("/adl/y") == 2);

  table.Log("loggable", telemetrytest::TestStructLoggable{3, 4});
  REQUIRE(Last<double>("/loggable/x") == 3);
  REQUIRE(Last<double>("/loggable/y") == 4);

  table.Log("valueStyle", telemetrytest::ValueStyle{5});
  REQUIRE(Last<double>("/valueStyle") == 5);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest TypedLoggableAndMismatchWarnings",
                 "[telemetry]") {
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/");
  table.Log("test", telemetrytest::TestStructLoggableType{1, 2});
  REQUIRE(table.GetTable("test").GetType() == "TestStructLoggableType");
  REQUIRE(table.GetTable("test").HasType());
  table.Log("test", telemetrytest::TestStructLoggableType{3, 4});

  auto typeValue =
      Last<wpi::telemetry::MockTelemetryBackend::LogStringValue>("/test/.type");
  REQUIRE(typeValue.value == "TestStructLoggableType");

  table.Log("adlTyped", telemetrytest::TestStructADLType{5, 6});
  REQUIRE(table.GetTable("adlTyped").GetType() == "TestStructADLType");

  REQUIRE_FALSE(table.GetTable("adlTyped").SetType("OtherType"));
  REQUIRE(warnings.size() == 1u);
  REQUIRE(warnings[0].first == "/adlTyped/");
  REQUIRE(warnings[0].second.find("table type mismatch") != std::string::npos);
}

TEST_CASE_METHOD(TelemetryTableTest, "TelemetryTableTest ResetClearsTableTypes",
                 "[telemetry]") {
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/");
  table.Log("test", telemetrytest::TestStructLoggableType{1, 2});
  REQUIRE(table.GetTable("test").GetType() == "TestStructLoggableType");
  mock->Clear();

  wpi::telemetry::TelemetryRegistry::Reset();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", mock);

  auto& resetTable = wpi::telemetry::TelemetryRegistry::GetTable("/");
  resetTable.Log("test", telemetrytest::TestStructLoggableType{3, 4});
  auto typeValue =
      Last<wpi::telemetry::MockTelemetryBackend::LogStringValue>("/test/.type");
  REQUIRE(typeValue.value == "TestStructLoggableType");
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest "
    "KeepDuplicatesAppliesMetadataAfterBackendResetDuringEntryLookup",
    "[telemetry]") {
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/rerouted/");
  std::promise<void> enteredIsDiscard;
  auto enteredIsDiscardFuture = enteredIsDiscard.get_future();
  std::promise<void> releaseIsDiscard;
  auto releaseIsDiscardFuture = releaseIsDiscard.get_future().share();
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "/rerouted/dups",
      std::make_shared<telemetrytest::BlockingIsDiscardBackend>(
          &enteredIsDiscard, releaseIsDiscardFuture, false));
  auto replacement = std::make_shared<wpi::telemetry::MockTelemetryBackend>();

  auto metadataFuture =
      std::async(std::launch::async, [&] { table.KeepDuplicates("dups"); });
  enteredIsDiscardFuture.wait();

  wpi::telemetry::TelemetryRegistry::RegisterBackend("/rerouted/dups",
                                                     replacement);
  table.Log("dups", 1.0);
  releaseIsDiscard.set_value();
  metadataFuture.get();

  const auto& actions = replacement->GetActions();
  REQUIRE_FALSE(actions.empty());
  CHECK(actions[0].path == "/rerouted/dups");
  CHECK(std::holds_alternative<
        wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue>(
      actions[0].value));
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest "
                 "SetPropertyAppliesMetadataAfterBackendResetDuringEntryLookup",
                 "[telemetry]") {
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/rerouted/");
  std::promise<void> enteredIsDiscard;
  auto enteredIsDiscardFuture = enteredIsDiscard.get_future();
  std::promise<void> releaseIsDiscard;
  auto releaseIsDiscardFuture = releaseIsDiscard.get_future().share();
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "/rerouted/prop",
      std::make_shared<telemetrytest::BlockingIsDiscardBackend>(
          &enteredIsDiscard, releaseIsDiscardFuture, false));
  auto replacement = std::make_shared<wpi::telemetry::MockTelemetryBackend>();

  auto metadataFuture = std::async(std::launch::async, [&] {
    table.SetProperty("prop", "unit", "\"count\"");
  });
  enteredIsDiscardFuture.wait();

  wpi::telemetry::TelemetryRegistry::RegisterBackend("/rerouted/prop",
                                                     replacement);
  table.Log("prop", 1.0);
  releaseIsDiscard.set_value();
  metadataFuture.get();

  const auto& actions = replacement->GetActions();
  REQUIRE_FALSE(actions.empty());
  CHECK(actions[0].path == "/rerouted/prop");
  auto property =
      std::get_if<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
          &actions[0].value);
  REQUIRE(property != nullptr);
  CHECK(property->key == "unit");
  CHECK(property->value == "\"count\"");
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest "
                 "SetTypeRejectsMismatchAfterBackendResetDuringTypePublication",
                 "[telemetry]") {
  auto replacement = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<telemetrytest::RegisteringTelemetryBackend>(
              replacement));
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/rerouted/");

  REQUIRE(table.SetType("FirstType"));
  CHECK_FALSE(table.SetType("OtherType"));
  CHECK(table.GetType() == "FirstType");
  auto typeValue =
      replacement
          ->GetLastValue<wpi::telemetry::MockTelemetryBackend::LogStringValue>(
              "/rerouted/.type");
  REQUIRE(typeValue.has_value());
  CHECK(typeValue->value == "FirstType");
  CHECK(typeValue->typeString == "string");
  REQUIRE(warnings.size() == 1u);
  CHECK(warnings[0].second.find("table type mismatch") != std::string::npos);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest TypedLoggableSetsTypeBeforeLoggingFields",
                 "[telemetry]") {
  auto& table = wpi::telemetry::TelemetryRegistry::GetTable("/");
  std::promise<void> enteredLogTo;
  auto enteredFuture = enteredLogTo.get_future();
  std::promise<void> releaseLogTo;
  auto releaseFuture = releaseLogTo.get_future().share();
  telemetrytest::BlockingTypedLoggable value{&enteredLogTo, releaseFuture};

  std::thread loggingThread{[&] { table.Log("blocked", value); }};
  enteredFuture.wait();

  auto& child = table.GetTable("blocked");
  CHECK(child.GetType() == "BlockingTypedLoggable");
  CHECK_FALSE(child.SetType("OtherType"));

  releaseLogTo.set_value();
  loggingThread.join();

  CHECK(Last<double>("/blocked/x") == 1.0);
  REQUIRE(warnings.size() == 1u);
  CHECK(warnings[0].first == "/blocked/");
  CHECK(warnings[0].second.find("table type mismatch") != std::string::npos);
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest StructLoggingRegistersSchemaAndLogsRawBytes",
    "[telemetry]") {
  wpi::telemetry::Log("point", telemetrytest::StructPoint{1.0, 2});

  auto* schema = mock->GetSchema("struct:telemetrytest.StructPoint");
  REQUIRE(schema != nullptr);
  REQUIRE(schema->type == "structschema");
  REQUIRE(schema->schemaString == "double x; int32 y");

  auto raw = Last<wpi::telemetry::MockTelemetryBackend::LogRawValue>("/point");
  REQUIRE(raw.typeString == "struct:telemetrytest.StructPoint");
  REQUIRE(raw.value.size() == 12u);
  auto unpacked = wpi::util::UnpackStruct<telemetrytest::StructPoint>(
      std::span<const uint8_t>{raw.value});
  REQUIRE(unpacked.x == 1.0);
  REQUIRE(unpacked.y == 2);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest StructSchemaRegistrationIsCachedPerEntry",
                 "[telemetry]") {
  auto countingMock = std::make_shared<telemetrytest::CountingSchemaBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", countingMock);

  wpi::telemetry::Log("point", telemetrytest::StructPoint{1.0, 2});
  REQUIRE(countingMock->GetSchemaAddCount() == 1);

  wpi::telemetry::Log("point", telemetrytest::StructPoint{3.0, 4});
  REQUIRE(countingMock->GetSchemaAddCount() == 1);

  wpi::telemetry::Log("other", telemetrytest::StructPoint{5.0, 6});
  REQUIRE(countingMock->GetSchemaAddCount() == 2);
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest StructArrayLoggingRegistersSchemaAndLogsRawBytes",
    "[telemetry]") {
  std::array<telemetrytest::StructPoint, 2> points{{{1.0, 2}, {3.0, 4}}};
  wpi::telemetry::Log("points", std::span<const telemetrytest::StructPoint>{
                                    points.data(), points.size()});

  auto* schema = mock->GetSchema("struct:telemetrytest.StructPoint");
  REQUIRE(schema != nullptr);
  auto raw = Last<wpi::telemetry::MockTelemetryBackend::LogRawValue>("/points");
  REQUIRE(raw.typeString == "struct:telemetrytest.StructPoint[]");
  REQUIRE(raw.value.size() == 24u);
  auto first = wpi::util::UnpackStruct<telemetrytest::StructPoint>(
      std::span<const uint8_t>{raw.value}.first(12));
  auto second = wpi::util::UnpackStruct<telemetrytest::StructPoint>(
      std::span<const uint8_t>{raw.value}.subspan(12));
  REQUIRE(first.x == 1.0);
  REQUIRE(first.y == 2);
  REQUIRE(second.x == 3.0);
  REQUIRE(second.y == 4);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest StructSchemaUsesQualifiedTablePathBackend",
                 "[telemetry]") {
  auto driveMock = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("/drive", driveMock);

  wpi::telemetry::GetTable("drive").Log("point",
                                        telemetrytest::StructPoint{1.0, 2});

  REQUIRE(mock->GetSchema("struct:telemetrytest.StructPoint") == nullptr);
  REQUIRE(driveMock->GetSchema("struct:telemetrytest.StructPoint") != nullptr);
  REQUIRE_FALSE(
      mock->GetLastValue<wpi::telemetry::MockTelemetryBackend::LogRawValue>(
              "/drive/point")
          .has_value());
  REQUIRE(driveMock
              ->GetLastValue<wpi::telemetry::MockTelemetryBackend::LogRawValue>(
                  "/drive/point")
              .has_value());
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest BackendPrefixSelectionAndCacheReset",
                 "[telemetry]") {
  wpi::telemetry::KeepDuplicates("drive/speed");
  wpi::telemetry::SetProperty("drive/speed", "unit", "\"m/s\"");
  auto& drive = wpi::telemetry::GetTable("drive");
  drive.Log("speed", 1.0);
  REQUIRE(Last<double>("/drive/speed") == 1.0);

  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "/arm", std::make_shared<wpi::telemetry::MockTelemetryBackend>());
  auto driveMock = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("/drive", driveMock);
  drive.Log("speed", 2.0);

  REQUIRE_FALSE(mock->GetLastValue<double>("/drive/speed").has_value());
  REQUIRE(std::holds_alternative<
          wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue>(
      driveMock->GetActions()[0].value));
  auto property =
      std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
          driveMock->GetActions()[1].value);
  REQUIRE(property.key == "unit");
  REQUIRE(property.value == "\"m/s\"");
  auto value = driveMock->GetLastValue<double>("/drive/speed");
  REQUIRE(value.has_value());
  REQUIRE(*value == 2.0);

  wpi::telemetry::TelemetryRegistry::Reset();
  auto resetMock = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", resetMock);
  drive.Log("speed", 3.0);

  auto resetValue = resetMock->GetLastValue<double>("/drive/speed");
  REQUIRE(resetValue.has_value());
  REQUIRE(*resetValue == 3.0);
  for (const auto& action : resetMock->GetActions()) {
    REQUIRE_FALSE(std::holds_alternative<
                  wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue>(
        action.value));
    REQUIRE_FALSE(std::holds_alternative<
                  wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
        action.value));
  }
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest GetBackendNormalizesPath", "[telemetry]") {
  auto driveMock = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("/drive", driveMock);

  REQUIRE(wpi::telemetry::TelemetryRegistry::GetBackend("drive/speed") ==
          driveMock);
  REQUIRE(wpi::telemetry::TelemetryRegistry::GetBackend("//drive//speed") ==
          driveMock);
  REQUIRE(wpi::telemetry::TelemetryRegistry::GetBackend("driver/speed") ==
          mock);

  wpi::telemetry::TelemetryRegistry::Reset();
  auto relativeDriveMock =
      std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("drive",
                                                     relativeDriveMock);

  REQUIRE(wpi::telemetry::TelemetryRegistry::GetBackend("drive/speed") ==
          relativeDriveMock);

  wpi::telemetry::TelemetryRegistry::Reset();
  auto repeatedSlashDriveMock =
      std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("//drive",
                                                     repeatedSlashDriveMock);

  REQUIRE(wpi::telemetry::TelemetryRegistry::GetBackend("drive/speed") ==
          repeatedSlashDriveMock);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest BackendPrefixRoutingUsesPathBoundaries",
                 "[telemetry]") {
  wpi::telemetry::Log("drive/speed", 1.0);
  wpi::telemetry::Log("driver/speed", 2.0);

  auto driveMock = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("/drive", driveMock);
  wpi::telemetry::Log("drive/speed", 3.0);
  wpi::telemetry::Log("driver/speed", 4.0);

  auto driveValue = driveMock->GetLastValue<double>("/drive/speed");
  REQUIRE(driveValue.has_value());
  CHECK(*driveValue == 3.0);
  CHECK_FALSE(driveMock->GetLastValue<double>("/driver/speed").has_value());
  CHECK_FALSE(mock->GetLastValue<double>("/drive/speed").has_value());
  auto driverValue = mock->GetLastValue<double>("/driver/speed");
  REQUIRE(driverValue.has_value());
  CHECK(*driverValue == 4.0);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest MissingBackendWarnsAndUsesDiscardBackend",
                 "[telemetry]") {
  wpi::telemetry::TelemetryRegistry::Reset();

  auto backend = wpi::telemetry::TelemetryRegistry::GetBackend("missing");
  wpi::telemetry::Log("missing", 1.0);

  CHECK(backend->GetEntry("/missing")->IsDiscard());
  bool foundWarning = false;
  for (auto&& warning : warnings) {
    if (warning.first == "/missing" &&
        warning.second.find("no backend for path") != std::string::npos) {
      foundWarning = true;
    }
  }
  CHECK(foundWarning);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest ReplacingBackendReleasesOrphanedBackend",
                 "[telemetry]") {
  std::weak_ptr<wpi::telemetry::MockTelemetryBackend> oldBackend;
  {
    auto backend = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
    oldBackend = backend;
    wpi::telemetry::TelemetryRegistry::RegisterBackend("", backend);
    wpi::telemetry::Log("replace/value", 1.0);
  }

  REQUIRE_FALSE(oldBackend.expired());

  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<wpi::telemetry::MockTelemetryBackend>());

  REQUIRE(oldBackend.expired());
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest ReplacingPrefixRetainsBackendRegisteredElsewhere",
    "[telemetry]") {
  std::weak_ptr<wpi::telemetry::MockTelemetryBackend> oldBackend;
  {
    auto backend = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
    oldBackend = backend;
    wpi::telemetry::TelemetryRegistry::RegisterBackend("", backend);
    wpi::telemetry::TelemetryRegistry::RegisterBackend("/drive", backend);
  }

  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "/drive", std::make_shared<wpi::telemetry::MockTelemetryBackend>());

  REQUIRE_FALSE(oldBackend.expired());

  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<wpi::telemetry::MockTelemetryBackend>());

  REQUIRE(oldBackend.expired());
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest MultiTelemetryBackendFansOut",
                 "[telemetry]") {
  wpi::telemetry::TelemetryRegistry::Reset();
  auto first = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  auto second = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<wpi::telemetry::MultiTelemetryBackend>(
              std::vector<std::shared_ptr<wpi::telemetry::TelemetryBackend>>{
                  first, second}));

  auto& table = wpi::telemetry::GetTable("multi");
  table.KeepDuplicates("speed");
  table.SetProperty("speed", "unit", "\"m/s\"");
  table.Log("speed", 4.5);
  table.Log("gyro", telemetrytest::TestStructLoggableType{1.0, 2.0});

  REQUIRE(first->GetLastValue<double>("/multi/speed") == 4.5);
  REQUIRE(second->GetLastValue<double>("/multi/speed") == 4.5);
  auto firstType =
      first->GetLastValue<wpi::telemetry::MockTelemetryBackend::LogStringValue>(
          "/multi/gyro/.type");
  auto secondType =
      second
          ->GetLastValue<wpi::telemetry::MockTelemetryBackend::LogStringValue>(
              "/multi/gyro/.type");
  REQUIRE(firstType.has_value());
  REQUIRE(secondType.has_value());
  REQUIRE(firstType->value == "TestStructLoggableType");
  REQUIRE(secondType->value == "TestStructLoggableType");

  REQUIRE(std::holds_alternative<
          wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue>(
      first->GetActions()[0].value));
  REQUIRE(std::holds_alternative<
          wpi::telemetry::MockTelemetryBackend::KeepDuplicatesValue>(
      second->GetActions()[0].value));
  auto firstProperty =
      std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
          first->GetActions()[1].value);
  auto secondProperty =
      std::get<wpi::telemetry::MockTelemetryBackend::SetPropertyValue>(
          second->GetActions()[1].value);
  REQUIRE(firstProperty.key == "unit");
  REQUIRE(firstProperty.value == "\"m/s\"");
  REQUIRE(secondProperty.key == "unit");
  REQUIRE(secondProperty.value == "\"m/s\"");
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest MultiTelemetryBackendRecreatesEntriesAfterRemove",
    "[telemetry]") {
  wpi::telemetry::TelemetryRegistry::Reset();
  auto child = std::make_shared<telemetrytest::GenerationTelemetryBackend>();
  auto multi = std::make_shared<wpi::telemetry::MultiTelemetryBackend>(
      std::vector<std::shared_ptr<wpi::telemetry::TelemetryBackend>>{child});
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", multi);

  wpi::telemetry::Log("rerouted", 1.0);
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "/rerouted", std::make_shared<wpi::telemetry::DiscardTelemetryBackend>());
  wpi::telemetry::TelemetryRegistry::RegisterBackend("/rerouted", multi);
  wpi::telemetry::Log("rerouted", 2.0);

  std::vector<int> expected{1, 2};
  REQUIRE(child->GetLogGenerations() == expected);
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest MultiTelemetryBackendRemovedEntrySkipsStaleEntry",
    "[telemetry]") {
  auto child = std::make_shared<telemetrytest::GenerationTelemetryBackend>();
  auto multi = std::make_shared<wpi::telemetry::MultiTelemetryBackend>(
      std::vector<std::shared_ptr<wpi::telemetry::TelemetryBackend>>{child});
  auto staleEntry = multi->GetEntry("/stale");

  multi->RemoveEntry("/stale");
  staleEntry->LogDouble(1.0, 0);
  multi->GetEntry("/stale")->LogDouble(2.0, 0);

  std::vector<int> expected{2};
  REQUIRE(child->GetLogGenerations() == expected);
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest MultiTelemetryBackendForwardsSchemas",
                 "[telemetry]") {
  wpi::telemetry::TelemetryRegistry::Reset();
  auto first = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  auto second = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  auto multi = std::make_shared<wpi::telemetry::MultiTelemetryBackend>(
      std::vector<std::shared_ptr<wpi::telemetry::TelemetryBackend>>{first,
                                                                     second});
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", multi);

  wpi::telemetry::Log("point", telemetrytest::StructPoint{1.0, 2});

  REQUIRE(multi->HasSchema("struct:telemetrytest.StructPoint"));
  REQUIRE(first->GetSchema("struct:telemetrytest.StructPoint") != nullptr);
  REQUIRE(second->GetSchema("struct:telemetrytest.StructPoint") != nullptr);
  REQUIRE(first
              ->GetLastValue<wpi::telemetry::MockTelemetryBackend::LogRawValue>(
                  "/point")
              .has_value());
  REQUIRE(second
              ->GetLastValue<wpi::telemetry::MockTelemetryBackend::LogRawValue>(
                  "/point")
              .has_value());
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest MultiTelemetryBackendAllDiscardSkipsTelemetryWork",
    "[telemetry]") {
  wpi::telemetry::TelemetryRegistry::Reset();
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "",
      std::make_shared<wpi::telemetry::MultiTelemetryBackend>(
          std::vector<std::shared_ptr<wpi::telemetry::TelemetryBackend>>{
              std::make_shared<wpi::telemetry::DiscardTelemetryBackend>(),
              std::make_shared<wpi::telemetry::DiscardTelemetryBackend>()}));

  wpi::telemetry::Log("formattable", telemetrytest::ThrowingFormattable{});
  wpi::telemetry::Log("loggable", telemetrytest::ThrowingLoggable{});

  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<wpi::telemetry::MultiTelemetryBackend>());

  wpi::telemetry::Log("formattable", telemetrytest::ThrowingFormattable{});
  wpi::telemetry::Log("loggable", telemetrytest::ThrowingLoggable{});
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest DiscardParentExpandsLoggableForNonDiscardDescendant",
    "[telemetry]") {
  wpi::telemetry::TelemetryRegistry::Reset();
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<wpi::telemetry::DiscardTelemetryBackend>());
  auto speedMock = std::make_shared<wpi::telemetry::MockTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("/robot/speed", speedMock);

  wpi::telemetry::Log("robot", telemetrytest::RobotSpeed{4.5});

  auto value = speedMock->GetLastValue<double>("/robot/speed");
  REQUIRE(value.has_value());
  REQUIRE(*value == 4.5);
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest BackendReplacementKeepsInFlightCachedEntryAlive",
    "[telemetry]") {
  auto state = std::make_shared<telemetrytest::BlockingBackendState>();
  auto oldBackend =
      std::make_shared<telemetrytest::BlockingTelemetryBackend>(state);
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", oldBackend);
  auto& table = wpi::telemetry::GetTable();
  table.Log("blocked", 1.0);
  REQUIRE(state->logs.load() == 1);

  oldBackend.reset();
  auto enteredFuture = state->enteredLog.get_future();
  state->block.store(true);
  std::thread loggingThread{[&] { table.Log("blocked", 2.0); }};
  enteredFuture.wait();

  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<wpi::telemetry::MockTelemetryBackend>());
  CHECK_FALSE(state->destroyed.load());

  state->releaseLog.set_value();
  loggingThread.join();

  CHECK(state->destroyed.load());
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest PublicEntryHandleKeepsBackendAliveAfterReplacement",
    "[telemetry]") {
  auto state = std::make_shared<telemetrytest::BlockingBackendState>();
  auto oldBackend =
      std::make_shared<telemetrytest::BlockingTelemetryBackend>(state);
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", oldBackend);

  auto entry = wpi::telemetry::TelemetryRegistry::GetEntry("direct");
  entry->LogDouble(1.0, 0);
  REQUIRE(state->logs.load() == 1);

  oldBackend.reset();
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "", std::make_shared<wpi::telemetry::MockTelemetryBackend>());

  CHECK_FALSE(state->destroyed.load());
  entry->LogDouble(2.0, 0);
  CHECK(state->logs.load() == 2);

  entry.reset();
  CHECK(state->destroyed.load());
}

TEST_CASE_METHOD(TelemetryTableTest,
                 "TelemetryTableTest ResetRemovesTrackedEntriesFromBackends",
                 "[telemetry]") {
  auto backend = std::make_shared<telemetrytest::ClosingTelemetryBackend>();
  wpi::telemetry::TelemetryRegistry::RegisterBackend("", backend);

  auto entry = wpi::telemetry::TelemetryRegistry::GetEntry("direct");
  entry->LogDouble(1.0, 0);
  REQUIRE(backend->GetLogs() == 1);

  wpi::telemetry::TelemetryRegistry::Reset();

  CHECK(backend->GetRemoves() == 1);
  entry->LogDouble(2.0, 0);
  CHECK(backend->GetLogs() == 1);
}

TEST_CASE_METHOD(
    TelemetryTableTest,
    "TelemetryTableTest DiscardBackendSkipsTelemetryWorkAndCacheResets",
    "[telemetry]") {
  wpi::telemetry::TelemetryRegistry::RegisterBackend(
      "/discard", std::make_shared<wpi::telemetry::DiscardTelemetryBackend>());
  auto& discard = wpi::telemetry::GetTable("discard");

  discard.KeepDuplicates("dups");
  discard.SetProperty("prop", "unit", "\"count\"");
  discard.Log("formattable", telemetrytest::ThrowingFormattable{});
  std::array<telemetrytest::ThrowingFormattable, 1> formattableArray;
  discard.Log(
      "formattableArray",
      std::span<const telemetrytest::ThrowingFormattable>{formattableArray});
  discard.Log("loggable", telemetrytest::ThrowingLoggable{});
  discard.Log("primitive", 1.0);
  std::array<uint8_t, 3> raw{1, 2, 3};
  discard.Log("raw", std::span<const uint8_t>{raw});

  REQUIRE(mock->GetActions().empty());

  wpi::telemetry::TelemetryRegistry::RegisterBackend("/discard", mock);
  discard.Log("primitive", 2.0);

  auto value = mock->GetLastValue<double>("/discard/primitive");
  REQUIRE(value.has_value());
  REQUIRE(*value == 2.0);
}
