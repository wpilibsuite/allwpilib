// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/telemetry/TelemetryTable.hpp"  // NOLINT(build/include_order)

#include <stdint.h>

#include <array>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/telemetry/DiscardTelemetryBackend.hpp"
#include "wpi/telemetry/MockTelemetryBackend.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace telemetrytest {
struct TestStructADL {
  double x = 0;
  double y = 0;
};

void LogTo(wpi::TelemetryTable& table, const TestStructADL& value) {
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

void LogTo(wpi::TelemetryTable& table, const TestStructADLType& value) {
  table.Log("x", value.x);
  table.Log("y", value.y);
}

struct TestStructLoggable : public wpi::TelemetryLoggable {
  double x = 0;
  double y = 0;

  TestStructLoggable(double x_, double y_) : x{x_}, y{y_} {}

  void LogTo(wpi::TelemetryTable& table) const override {
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

struct ValueStyle {
  double value = 0;
};

void LogValueTo(wpi::TelemetryTable& table, std::string_view name,
                const ValueStyle& value) {
  table.Log(name, value.value);
}

struct StructPoint {
  double x = 0;
  int32_t y = 0;
};

struct Formattable {
  int value = 0;
};

struct ThrowingFormattable {};

struct ThrowingLoggable : public wpi::TelemetryLoggable {
  void LogTo(wpi::TelemetryTable& table) const override {
    (void)table;
    throw std::logic_error{"LogTo should not run"};
  }
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
struct fmt::formatter<telemetrytest::Formattable> : fmt::formatter<int> {
  auto format(const telemetrytest::Formattable& value,
              format_context& ctx) const {
    return fmt::formatter<int>::format(value.value, ctx);
  }
};

template <>
struct fmt::formatter<telemetrytest::ThrowingFormattable>
    : fmt::formatter<int> {
  auto format(const telemetrytest::ThrowingFormattable& value,
              format_context& ctx) const {
    (void)value;
    throw std::logic_error{"format should not run"};
    return fmt::formatter<int>::format(0, ctx);
  }
};

struct TelemetryTableTest : public ::testing::Test {
  void SetUp() override {
    warnings.clear();
    wpi::TelemetryRegistry::Reset();
    wpi::TelemetryRegistry::SetReportWarning(
        [this](std::string_view path, std::string_view msg) {
          warnings.emplace_back(path, msg);
        });
    wpi::TelemetryRegistry::RegisterBackend("", mock);
  }

  void TearDown() override {
    wpi::TelemetryRegistry::SetReportWarning(nullptr);
    wpi::TelemetryRegistry::Reset();
  }

  template <typename T>
  T Last(std::string_view path) {
    auto value = mock->GetLastValue<T>(path);
    if (!value) {
      ADD_FAILURE() << "No last value at " << path
                    << " with requested variant type"
                    << (mock->GetLastAction(path)
                            ? fmt::format(
                                  "; actual variant index {}",
                                  mock->GetLastAction(path)->value.index())
                            : "; no action at that path");
      return T{};
    }
    return *value;
  }

  std::shared_ptr<wpi::MockTelemetryBackend> mock =
      std::make_shared<wpi::MockTelemetryBackend>();
  std::vector<std::pair<std::string, std::string>> warnings;
};

TEST_F(TelemetryTableTest, TablePathNormalizationAndCaching) {
  auto& root = wpi::TelemetryRegistry::GetTable("");
  auto& drive = wpi::TelemetryRegistry::GetTable("drive");
  ASSERT_EQ(root.GetPath(), "/");
  ASSERT_EQ(drive.GetPath(), "/drive/");
  ASSERT_EQ(&drive, &wpi::TelemetryRegistry::GetTable("/drive/"));
  ASSERT_EQ(&drive, &wpi::Telemetry::GetTable("drive"));
  ASSERT_EQ(&drive.GetTable("left"),
            &wpi::TelemetryRegistry::GetTable("/drive/left/"));
}

TEST_F(TelemetryTableTest, FacadeMetadataAndPrimitiveLogging) {
  wpi::Telemetry::KeepDuplicates("loops");
  wpi::Telemetry::SetProperty("loops", "unit", "\"count\"");
  wpi::Telemetry::Log("enabled", true);
  wpi::Telemetry::Log("short", int16_t{2});
  wpi::Telemetry::Log("int", int32_t{3});
  wpi::Telemetry::Log("long", int64_t{4});
  wpi::Telemetry::Log("float", 5.0f);
  wpi::Telemetry::Log("double", 6.0);
  wpi::Telemetry::Log("state", std::string_view{"ready"});
  wpi::Telemetry::Log("json", std::string_view{"{\"ok\":true}"},
                      std::string_view{"json"});

  ASSERT_EQ(mock->GetActions()[0].path, "/loops");
  ASSERT_TRUE(
      std::holds_alternative<wpi::MockTelemetryBackend::KeepDuplicatesValue>(
          mock->GetActions()[0].value));
  auto property = std::get<wpi::MockTelemetryBackend::SetPropertyValue>(
      mock->GetActions()[1].value);
  ASSERT_EQ(property.key, "unit");
  ASSERT_EQ(property.value, "\"count\"");
  ASSERT_EQ(Last<bool>("/enabled"), true);
  ASSERT_EQ(Last<int16_t>("/short"), 2);
  ASSERT_EQ(Last<int32_t>("/int"), 3);
  ASSERT_EQ(Last<int64_t>("/long"), 4);
  ASSERT_EQ(Last<float>("/float"), 5.0f);
  ASSERT_EQ(Last<double>("/double"), 6.0);

  auto state = Last<wpi::MockTelemetryBackend::LogStringValue>("/state");
  ASSERT_EQ(state.value, "ready");
  ASSERT_EQ(state.typeString, "string");
  auto json = Last<wpi::MockTelemetryBackend::LogStringValue>("/json");
  ASSERT_EQ(json.value, "{\"ok\":true}");
  ASSERT_EQ(json.typeString, "json");
}

TEST_F(TelemetryTableTest, ArrayLoggingCopiesInputs) {
  std::array<bool, 2> bools{true, false};
  std::array<int16_t, 2> shorts{1, 2};
  std::array<int32_t, 2> ints{3, 4};
  std::array<int64_t, 2> longs{5, 6};
  std::array<float, 2> floats{7.0f, 8.0f};
  std::array<double, 2> doubles{9.0, 10.0};
  std::array<std::string_view, 2> strings{"a", "b"};
  std::array<uint8_t, 2> raw{11, 12};

  wpi::Telemetry::Log("bools", std::span<const bool>{bools});
  wpi::Telemetry::Log("shorts", std::span<const int16_t>{shorts});
  wpi::Telemetry::Log("ints", std::span<const int32_t>{ints});
  wpi::Telemetry::Log("longs", std::span<const int64_t>{longs});
  wpi::Telemetry::Log("floats", std::span<const float>{floats});
  wpi::Telemetry::Log("doubles", std::span<const double>{doubles});
  wpi::Telemetry::Log("strings", std::span<const std::string_view>{strings});
  wpi::Telemetry::Log("raw", std::span<const uint8_t>{raw},
                      std::string_view{"bytes"});
  wpi::Telemetry::Log("initializerBools", {true, false});
  wpi::Telemetry::Log("initializerShorts", {int16_t{1}, int16_t{2}});
  wpi::Telemetry::Log("initializerInts", {int32_t{3}, int32_t{4}});
  wpi::Telemetry::Log("initializerLongs", {int64_t{5}, int64_t{6}});
  wpi::Telemetry::Log("initializerFloats", {7.0f, 8.0f});
  wpi::Telemetry::Log("initializerDoubles", {9.0, 10.0});

  bools[0] = false;
  shorts[0] = 99;
  ints[0] = 99;
  longs[0] = 99;
  floats[0] = 99;
  doubles[0] = 99;
  strings[0] = "changed";
  raw[0] = 99;

  ASSERT_EQ(
      Last<wpi::MockTelemetryBackend::LogBooleanArrayValue>("/bools").value,
      (std::vector<int>{1, 0}));
  ASSERT_EQ(Last<std::vector<int16_t>>("/shorts"),
            (std::vector<int16_t>{1, 2}));
  ASSERT_EQ(Last<std::vector<int32_t>>("/ints"), (std::vector<int32_t>{3, 4}));
  ASSERT_EQ(Last<std::vector<int64_t>>("/longs"), (std::vector<int64_t>{5, 6}));
  ASSERT_EQ(Last<std::vector<float>>("/floats"),
            (std::vector<float>{7.0f, 8.0f}));
  ASSERT_EQ(Last<std::vector<double>>("/doubles"),
            (std::vector<double>{9.0, 10.0}));
  ASSERT_EQ(Last<std::vector<std::string>>("/strings"),
            (std::vector<std::string>{"a", "b"}));
  auto rawValue = Last<wpi::MockTelemetryBackend::LogRawValue>("/raw");
  ASSERT_EQ(rawValue.value, (std::vector<uint8_t>{11, 12}));
  ASSERT_EQ(rawValue.typeString, "bytes");
  ASSERT_EQ(
      Last<wpi::MockTelemetryBackend::LogBooleanArrayValue>("/initializerBools")
          .value,
      (std::vector<int>{1, 0}));
  ASSERT_EQ(Last<std::vector<int16_t>>("/initializerShorts"),
            (std::vector<int16_t>{1, 2}));
  ASSERT_EQ(Last<std::vector<int32_t>>("/initializerInts"),
            (std::vector<int32_t>{3, 4}));
  ASSERT_EQ(Last<std::vector<int64_t>>("/initializerLongs"),
            (std::vector<int64_t>{5, 6}));
  ASSERT_EQ(Last<std::vector<float>>("/initializerFloats"),
            (std::vector<float>{7.0f, 8.0f}));
  ASSERT_EQ(Last<std::vector<double>>("/initializerDoubles"),
            (std::vector<double>{9.0, 10.0}));
}

TEST_F(TelemetryTableTest, GenericDispatch) {
  auto& table = wpi::Telemetry::GetTable();
  table.Log("boolValue", true);
  table.Log("integralValue", int64_t{7});
  table.Log("floatingValue", 1.25);
  table.Log("stringValue", "hello");
  table.Log("boundedArray", std::array<int32_t, 2>{1, 2});
  table.Log("formattable", telemetrytest::Formattable{42});
  std::array<telemetrytest::Formattable, 2> formattableArray{{{1}, {2}}};
  table.Log("formattableArray",
            std::span<const telemetrytest::Formattable>{formattableArray});

  ASSERT_EQ(Last<bool>("/boolValue"), true);
  ASSERT_EQ(Last<int64_t>("/integralValue"), 7);
  ASSERT_EQ(Last<double>("/floatingValue"), 1.25);
  ASSERT_EQ(
      Last<wpi::MockTelemetryBackend::LogStringValue>("/stringValue").value,
      "hello");
  ASSERT_EQ(Last<std::vector<int32_t>>("/boundedArray"),
            (std::vector<int32_t>{1, 2}));
  ASSERT_EQ(
      Last<wpi::MockTelemetryBackend::LogStringValue>("/formattable").value,
      "42");
  ASSERT_EQ(Last<std::vector<std::string>>("/formattableArray"),
            (std::vector<std::string>{"1", "2"}));
}

TEST_F(TelemetryTableTest, LoggableAndADLObjects) {
  auto& table = wpi::TelemetryRegistry::GetTable("/");
  table.Log("adl", telemetrytest::TestStructADL{1, 2});
  ASSERT_EQ(Last<double>("/adl/x"), 1);
  ASSERT_EQ(Last<double>("/adl/y"), 2);

  table.Log("loggable", telemetrytest::TestStructLoggable{3, 4});
  ASSERT_EQ(Last<double>("/loggable/x"), 3);
  ASSERT_EQ(Last<double>("/loggable/y"), 4);

  table.Log("valueStyle", telemetrytest::ValueStyle{5});
  ASSERT_EQ(Last<double>("/valueStyle"), 5);
}

TEST_F(TelemetryTableTest, TypedLoggableAndMismatchWarnings) {
  auto& table = wpi::TelemetryRegistry::GetTable("/");
  table.Log("test", telemetrytest::TestStructLoggableType{1, 2});
  ASSERT_EQ(table.GetTable("test").GetType(), "TestStructLoggableType");
  ASSERT_TRUE(table.GetTable("test").HasType());
  table.Log("test", telemetrytest::TestStructLoggableType{3, 4});

  auto typeValue =
      Last<wpi::MockTelemetryBackend::LogStringValue>("/test/.type");
  ASSERT_EQ(typeValue.value, "TestStructLoggableType");

  table.Log("adlTyped", telemetrytest::TestStructADLType{5, 6});
  ASSERT_EQ(table.GetTable("adlTyped").GetType(), "TestStructADLType");

  ASSERT_FALSE(table.GetTable("adlTyped").SetType("OtherType"));
  ASSERT_EQ(warnings.size(), 1u);
  ASSERT_EQ(warnings[0].first, "/adlTyped/");
  ASSERT_NE(warnings[0].second.find("table type mismatch"), std::string::npos);
}

TEST_F(TelemetryTableTest, StructLoggingRegistersSchemaAndLogsRawBytes) {
  wpi::Telemetry::Log("point", telemetrytest::StructPoint{1.0, 2});

  auto* schema = mock->GetSchema("struct:telemetrytest.StructPoint");
  ASSERT_NE(schema, nullptr);
  ASSERT_EQ(schema->type, "structschema");
  ASSERT_EQ(schema->schemaString, "double x; int32 y");

  auto raw = Last<wpi::MockTelemetryBackend::LogRawValue>("/point");
  ASSERT_EQ(raw.typeString, "struct:telemetrytest.StructPoint");
  ASSERT_EQ(raw.value.size(), 12u);
  auto unpacked = wpi::util::UnpackStruct<telemetrytest::StructPoint>(
      std::span<const uint8_t>{raw.value});
  ASSERT_EQ(unpacked.x, 1.0);
  ASSERT_EQ(unpacked.y, 2);
}

TEST_F(TelemetryTableTest, StructArrayLoggingRegistersSchemaAndLogsRawBytes) {
  std::array<telemetrytest::StructPoint, 2> points{{{1.0, 2}, {3.0, 4}}};
  wpi::Telemetry::Log("points", std::span<const telemetrytest::StructPoint>{
                                    points.data(), points.size()});

  auto* schema = mock->GetSchema("struct:telemetrytest.StructPoint");
  ASSERT_NE(schema, nullptr);
  auto raw = Last<wpi::MockTelemetryBackend::LogRawValue>("/points");
  ASSERT_EQ(raw.typeString, "struct:telemetrytest.StructPoint[]");
  ASSERT_EQ(raw.value.size(), 24u);
  auto first = wpi::util::UnpackStruct<telemetrytest::StructPoint>(
      std::span<const uint8_t>{raw.value}.first(12));
  auto second = wpi::util::UnpackStruct<telemetrytest::StructPoint>(
      std::span<const uint8_t>{raw.value}.subspan(12));
  ASSERT_EQ(first.x, 1.0);
  ASSERT_EQ(first.y, 2);
  ASSERT_EQ(second.x, 3.0);
  ASSERT_EQ(second.y, 4);
}

TEST_F(TelemetryTableTest, StructSchemaUsesQualifiedTablePathBackend) {
  auto driveMock = std::make_shared<wpi::MockTelemetryBackend>();
  wpi::TelemetryRegistry::RegisterBackend("/drive", driveMock);

  wpi::Telemetry::GetTable("drive").Log("point",
                                        telemetrytest::StructPoint{1.0, 2});

  ASSERT_EQ(mock->GetSchema("struct:telemetrytest.StructPoint"), nullptr);
  ASSERT_NE(driveMock->GetSchema("struct:telemetrytest.StructPoint"), nullptr);
  ASSERT_FALSE(mock->GetLastValue<wpi::MockTelemetryBackend::LogRawValue>(
      "/drive/point"));
  ASSERT_TRUE(driveMock->GetLastValue<wpi::MockTelemetryBackend::LogRawValue>(
      "/drive/point"));
}

TEST_F(TelemetryTableTest, BackendPrefixSelectionAndCacheReset) {
  auto& drive = wpi::Telemetry::GetTable("drive");
  drive.Log("speed", 1.0);
  ASSERT_EQ(Last<double>("/drive/speed"), 1.0);

  mock->Clear();
  auto driveMock = std::make_shared<wpi::MockTelemetryBackend>();
  wpi::TelemetryRegistry::RegisterBackend("/drive", driveMock);
  drive.Log("speed", 2.0);

  ASSERT_FALSE(mock->GetLastValue<double>("/drive/speed"));
  auto value = driveMock->GetLastValue<double>("/drive/speed");
  ASSERT_TRUE(value);
  ASSERT_EQ(*value, 2.0);
}

TEST_F(TelemetryTableTest, DiscardBackendSkipsTelemetryWorkAndCacheResets) {
  wpi::TelemetryRegistry::RegisterBackend(
      "/discard", std::make_shared<wpi::DiscardTelemetryBackend>());
  auto& discard = wpi::Telemetry::GetTable("discard");

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

  ASSERT_TRUE(mock->GetActions().empty());

  wpi::TelemetryRegistry::RegisterBackend("/discard", mock);
  discard.Log("primitive", 2.0);

  auto value = mock->GetLastValue<double>("/discard/primitive");
  ASSERT_TRUE(value);
  ASSERT_EQ(*value, 2.0);
}
