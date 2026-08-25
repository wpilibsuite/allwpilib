// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/DataLogTelemetryBackend.hpp"

#include <stdint.h>

#include <array>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/datalog/DataLogReader.hpp"
#include "wpi/datalog/DataLogWriter.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/proto/Translation2dProto.hpp"
#include "wpi/math/geometry/struct/Translation2dStruct.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/Logger.hpp"
#include "wpi/util/MemoryBuffer.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/raw_ostream.hpp"
#include "wpi/util/struct/Struct.hpp"

class DataLogTelemetryBackendTest {
 public:
  struct EntryData {
    std::string type;
    std::string metadata;
    std::vector<std::vector<uint8_t>> records;
  };

  struct LogSnapshot {
    std::unordered_map<std::string, EntryData> entries;
  };

  DataLogTelemetryBackendTest()
      : log{msglog, std::make_unique<wpi::util::raw_uvector_ostream>(data)},
        backend{std::make_shared<wpi::backend::DataLogTelemetryBackend>(
            log, "/Telemetry")} {
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::telemetry::TelemetryRegistry::RegisterBackend("", backend);
  }

  ~DataLogTelemetryBackendTest() {
    wpi::telemetry::TelemetryRegistry::SetReportWarning(nullptr);
    wpi::telemetry::TelemetryRegistry::Reset();
  }

  LogSnapshot ReadSnapshot() {
    log.Flush();

    wpi::log::DataLogReader reader{
        wpi::util::MemoryBuffer::GetMemBufferCopy(data, "test")};
    CHECK(reader.IsValid());

    std::unordered_map<int, std::string> names;
    LogSnapshot snapshot;
    for (const auto& record : reader) {
      if (record.IsStart()) {
        wpi::log::StartRecordData start;
        CHECK(record.GetStartData(&start));
        names.emplace(start.entry, start.name);
        auto& entry = snapshot.entries[std::string{start.name}];
        entry.type = start.type;
        entry.metadata = start.metadata;
      } else if (record.IsSetMetadata()) {
        wpi::log::MetadataRecordData metadata;
        CHECK(record.GetSetMetadataData(&metadata));
        auto it = names.find(metadata.entry);
        if (it != names.end()) {
          snapshot.entries[it->second].metadata = metadata.metadata;
        }
      } else if (!record.IsControl()) {
        auto it = names.find(record.GetEntry());
        if (it != names.end()) {
          auto raw = record.GetRaw();
          snapshot.entries[it->second].records.emplace_back(raw.begin(),
                                                            raw.end());
        }
      }
    }
    return snapshot;
  }

  static const EntryData& Entry(const LogSnapshot& snapshot,
                                std::string_view name) {
    return snapshot.entries.at(std::string{"/Telemetry/"} + std::string{name});
  }

  static const std::vector<uint8_t>& Last(const EntryData& entry) {
    REQUIRE_FALSE(entry.records.empty());
    return entry.records.back();
  }

  static bool HasEntryWithType(const LogSnapshot& snapshot,
                               std::string_view type) {
    for (const auto& item : snapshot.entries) {
      if (item.second.type == type) {
        return true;
      }
    }
    return false;
  }

  static bool DecodeBoolean(const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    bool value = false;
    CHECK(record.GetBoolean(&value));
    return value;
  }

  static int64_t DecodeInteger(const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    int64_t value = 0;
    CHECK(record.GetInteger(&value));
    return value;
  }

  static float DecodeFloat(const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    float value = 0.0f;
    CHECK(record.GetFloat(&value));
    return value;
  }

  static double DecodeDouble(const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    double value = 0.0;
    CHECK(record.GetDouble(&value));
    return value;
  }

  static std::string DecodeString(const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    std::string_view value;
    CHECK(record.GetString(&value));
    return std::string{value};
  }

  static std::vector<int> DecodeBooleanArray(const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    std::vector<int> value;
    CHECK(record.GetBooleanArray(&value));
    return value;
  }

  static std::vector<int64_t> DecodeIntegerArray(
      const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    std::vector<int64_t> value;
    CHECK(record.GetIntegerArray(&value));
    return value;
  }

  static std::vector<float> DecodeFloatArray(const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    std::vector<float> value;
    CHECK(record.GetFloatArray(&value));
    return value;
  }

  static std::vector<double> DecodeDoubleArray(
      const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    std::vector<double> value;
    CHECK(record.GetDoubleArray(&value));
    return value;
  }

  static std::vector<std::string> DecodeStringArray(
      const std::vector<uint8_t>& raw) {
    wpi::log::DataLogRecord record{1, 0, raw};
    std::vector<std::string_view> views;
    CHECK(record.GetStringArray(&views));
    return {views.begin(), views.end()};
  }

  static wpi::math::Translation2d DecodeTranslation(
      const std::vector<uint8_t>& raw) {
    return wpi::util::UnpackStruct<wpi::math::Translation2d>(
        std::span<const uint8_t>{raw.data(), raw.size()});
  }

  static std::vector<wpi::math::Translation2d> DecodeTranslationArray(
      const std::vector<uint8_t>& raw) {
    constexpr size_t STRUCT_SIZE =
        wpi::util::Struct<wpi::math::Translation2d>::GetSize();
    REQUIRE(0u == raw.size() % STRUCT_SIZE);
    std::vector<wpi::math::Translation2d> values;
    for (size_t offset = 0; offset < raw.size(); offset += STRUCT_SIZE) {
      values.emplace_back(wpi::util::UnpackStruct<wpi::math::Translation2d>(
          std::span<const uint8_t>{raw.data() + offset, STRUCT_SIZE}));
    }
    return values;
  }

  static wpi::math::Translation2d DecodeTranslationProto(
      const std::vector<uint8_t>& raw) {
    wpi::util::ProtobufMessage<wpi::math::Translation2d> msg;
    auto value = msg.Unpack(raw);
    REQUIRE(value.has_value());
    return value.value_or(wpi::math::Translation2d{});
  }

  static void ExpectTranslationEq(const wpi::math::Translation2d& expected,
                                  const wpi::math::Translation2d& actual) {
    CHECK(expected.X().value() == actual.X().value());
    CHECK(expected.Y().value() == actual.Y().value());
  }

  wpi::util::Logger msglog;
  std::vector<uint8_t> data;
  wpi::log::DataLogWriter log;
  std::shared_ptr<wpi::backend::DataLogTelemetryBackend> backend;
};

TEST_CASE_METHOD(DataLogTelemetryBackendTest,
                 "DataLogTelemetryBackendTest LogsScalarDataTypes",
                 "[wpilibc][telemetry]") {
  wpi::telemetry::Log("boolean", true);
  wpi::telemetry::Log("byte", int8_t{2});
  wpi::telemetry::Log("short", int16_t{3});
  wpi::telemetry::Log("int", int32_t{4});
  wpi::telemetry::Log("long", int64_t{5});
  wpi::telemetry::Log("float", 6.25f);
  wpi::telemetry::Log("double", 7.5);
  wpi::telemetry::Log("string", "ready");
  wpi::telemetry::Log("json", std::string_view{"{\"ok\":true}"},
                      std::string_view{"json"});

  auto snapshot = ReadSnapshot();

  CHECK("boolean" == Entry(snapshot, "boolean").type);
  CHECK(DecodeBoolean(Last(Entry(snapshot, "boolean"))));
  CHECK("int64" == Entry(snapshot, "byte").type);
  CHECK(2 == DecodeInteger(Last(Entry(snapshot, "byte"))));
  CHECK("int64" == Entry(snapshot, "short").type);
  CHECK(3 == DecodeInteger(Last(Entry(snapshot, "short"))));
  CHECK("int64" == Entry(snapshot, "int").type);
  CHECK(4 == DecodeInteger(Last(Entry(snapshot, "int"))));
  CHECK("int64" == Entry(snapshot, "long").type);
  CHECK(5 == DecodeInteger(Last(Entry(snapshot, "long"))));
  CHECK("float" == Entry(snapshot, "float").type);
  CHECK(6.25f == DecodeFloat(Last(Entry(snapshot, "float"))));
  CHECK("double" == Entry(snapshot, "double").type);
  CHECK(7.5 == DecodeDouble(Last(Entry(snapshot, "double"))));
  CHECK("string" == Entry(snapshot, "string").type);
  CHECK("ready" == DecodeString(Last(Entry(snapshot, "string"))));
  CHECK("json" == Entry(snapshot, "json").type);
  CHECK("{\"ok\":true}" == DecodeString(Last(Entry(snapshot, "json"))));
}

TEST_CASE_METHOD(DataLogTelemetryBackendTest,
                 "DataLogTelemetryBackendTest LogsArrayAndRawDataTypes",
                 "[wpilibc][telemetry]") {
  const bool boolValues[] = {true, false};
  const int16_t shortValues[] = {1, 2};
  const int32_t intValues[] = {3, 4};
  const int64_t longValues[] = {5, 6};
  const float floatValues[] = {7.25f, 8.5f};
  const double doubleValues[] = {9.25, 10.5};
  const std::string stringValues[] = {"a", "b"};
  const std::string_view stringViewValues[] = {"c", "d"};
  const uint8_t rawValues[] = {11, 12, 13};
  const uint8_t customRawValues[] = {14, 15};

  wpi::telemetry::Log("booleans", boolValues);
  wpi::telemetry::Log("shorts", shortValues);
  wpi::telemetry::Log("ints", intValues);
  wpi::telemetry::Log("longs", longValues);
  wpi::telemetry::Log("floats", floatValues);
  wpi::telemetry::Log("doubles", doubleValues);
  wpi::telemetry::Log("strings", stringValues);
  wpi::telemetry::Log("stringViews", stringViewValues);
  wpi::telemetry::Log("raw", rawValues);
  wpi::telemetry::Log("customRaw", std::span<const uint8_t>{customRawValues},
                      "custom");

  auto snapshot = ReadSnapshot();

  CHECK("boolean[]" == Entry(snapshot, "booleans").type);
  CHECK((std::vector<int>{1, 0}) ==
        DecodeBooleanArray(Last(Entry(snapshot, "booleans"))));
  CHECK("int64[]" == Entry(snapshot, "shorts").type);
  CHECK((std::vector<int64_t>{1, 2}) ==
        DecodeIntegerArray(Last(Entry(snapshot, "shorts"))));
  CHECK("int64[]" == Entry(snapshot, "ints").type);
  CHECK((std::vector<int64_t>{3, 4}) ==
        DecodeIntegerArray(Last(Entry(snapshot, "ints"))));
  CHECK("int64[]" == Entry(snapshot, "longs").type);
  CHECK((std::vector<int64_t>{5, 6}) ==
        DecodeIntegerArray(Last(Entry(snapshot, "longs"))));
  CHECK("float[]" == Entry(snapshot, "floats").type);
  CHECK((std::vector<float>{7.25f, 8.5f}) ==
        DecodeFloatArray(Last(Entry(snapshot, "floats"))));
  CHECK("double[]" == Entry(snapshot, "doubles").type);
  CHECK((std::vector<double>{9.25, 10.5}) ==
        DecodeDoubleArray(Last(Entry(snapshot, "doubles"))));
  CHECK("string[]" == Entry(snapshot, "strings").type);
  CHECK((std::vector<std::string>{"a", "b"}) ==
        DecodeStringArray(Last(Entry(snapshot, "strings"))));
  CHECK("string[]" == Entry(snapshot, "stringViews").type);
  CHECK((std::vector<std::string>{"c", "d"}) ==
        DecodeStringArray(Last(Entry(snapshot, "stringViews"))));
  CHECK("raw" == Entry(snapshot, "raw").type);
  CHECK((std::vector<uint8_t>{11, 12, 13}) == Last(Entry(snapshot, "raw")));
  CHECK("custom" == Entry(snapshot, "customRaw").type);
  CHECK((std::vector<uint8_t>{14, 15}) == Last(Entry(snapshot, "customRaw")));
}

TEST_CASE_METHOD(DataLogTelemetryBackendTest,
                 "DataLogTelemetryBackendTest LogsStructAndProtobufDataTypes",
                 "[wpilibc][telemetry]") {
  const wpi::math::Translation2d value{1.25_m, 2.5_m};
  const std::array<wpi::math::Translation2d, 2> array{
      value, wpi::math::Translation2d{3.75_m, 4.5_m}};
  wpi::util::ProtobufMessage<wpi::math::Translation2d> msg;
  const std::string structType{std::string_view{
      wpi::util::GetStructTypeString<wpi::math::Translation2d>()}};
  const std::string protoType = msg.GetTypeString();

  wpi::telemetry::Log("translation", value);
  wpi::telemetry::Log("translations", std::span<const wpi::math::Translation2d>{
                                          array.data(), array.size()});
  wpi::telemetry::Log("translationProto", value,
                      wpi::util::ProtobufMessage<wpi::math::Translation2d>{});

  auto snapshot = ReadSnapshot();

  CHECK(structType == Entry(snapshot, "translation").type);
  ExpectTranslationEq(value,
                      DecodeTranslation(Last(Entry(snapshot, "translation"))));
  CHECK(structType + "[]" == Entry(snapshot, "translations").type);
  auto decodedArray =
      DecodeTranslationArray(Last(Entry(snapshot, "translations")));
  REQUIRE(2u == decodedArray.size());
  ExpectTranslationEq(array[0], decodedArray[0]);
  ExpectTranslationEq(array[1], decodedArray[1]);
  CHECK(protoType == Entry(snapshot, "translationProto").type);
  ExpectTranslationEq(
      value, DecodeTranslationProto(Last(Entry(snapshot, "translationProto"))));

  CHECK("structschema" == snapshot.entries.at("/.schema/" + structType).type);
  CHECK(HasEntryWithType(snapshot, "proto:FileDescriptorProto"));
}

TEST_CASE_METHOD(DataLogTelemetryBackendTest,
                 "DataLogTelemetryBackendTest AppliesTelemetryProperties",
                 "[wpilibc][telemetry]") {
  wpi::telemetry::SetProperty("speed", "min", "0");
  wpi::telemetry::SetProperty("speed", "unit", "\"m/s\"");
  wpi::telemetry::Log("speed", 4.0);
  wpi::telemetry::SetProperty("speed", "max", "10");

  auto snapshot = ReadSnapshot();
  const auto& speed = Entry(snapshot, "speed");

  auto metadata = wpi::util::json::parse(speed.metadata);
  REQUIRE(metadata);
  REQUIRE((*metadata)["min"].is_number());
  REQUIRE((*metadata)["max"].is_number());
  REQUIRE((*metadata)["unit"].is_string());
  CHECK(0.0 == (*metadata)["min"].get_number());
  CHECK(10.0 == (*metadata)["max"].get_number());
  CHECK("m/s" == (*metadata)["unit"].get_string());
}

TEST_CASE_METHOD(DataLogTelemetryBackendTest,
                 "DataLogTelemetryBackendTest HonorsKeepDuplicates",
                 "[wpilibc][telemetry]") {
  wpi::telemetry::Log("normal", int64_t{1});
  wpi::telemetry::Log("normal", int64_t{1});
  wpi::telemetry::KeepDuplicates("duplicates");
  wpi::telemetry::Log("duplicates", int64_t{2});
  wpi::telemetry::Log("duplicates", int64_t{2});

  auto snapshot = ReadSnapshot();

  CHECK(1u == Entry(snapshot, "normal").records.size());
  CHECK(2u == Entry(snapshot, "duplicates").records.size());
}

TEST_CASE_METHOD(DataLogTelemetryBackendTest,
                 "DataLogTelemetryBackendTest RemovedEntryDoesNotRecreateEntry",
                 "[wpilibc][telemetry]") {
  auto staleEntry = backend->GetEntry("/stale");
  backend->RemoveEntry("/stale");

  staleEntry->LogDouble(1.25);
  backend->GetEntry("/stale")->LogDouble(2.5);

  auto snapshot = ReadSnapshot();
  const auto& stale = Entry(snapshot, "stale");
  CHECK(1u == stale.records.size());
  CHECK(2.5 == DecodeDouble(Last(stale)));
}

TEST_CASE_METHOD(
    DataLogTelemetryBackendTest,
    "DataLogTelemetryBackendTest TypeMismatchWarningAllowsSameEntryReentry",
    "[wpilibc][telemetry]") {
  std::vector<std::string> warnings;
  bool nested = false;
  wpi::telemetry::TelemetryRegistry::SetReportWarning(
      [&](std::string_view path, std::string_view msg) {
        CHECK("/Telemetry/value" == path);
        CHECK("type mismatch" == msg);
        warnings.emplace_back(path);
        if (!nested) {
          nested = true;
          wpi::telemetry::Log("value", "again");
        }
      });

  wpi::telemetry::Log("value", int64_t{1});
  wpi::telemetry::Log("value", true);

  CHECK((std::vector<std::string>{"/Telemetry/value", "/Telemetry/value"}) ==
        warnings);
}
