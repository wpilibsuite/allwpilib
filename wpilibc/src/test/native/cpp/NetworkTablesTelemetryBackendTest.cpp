// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/NetworkTablesTelemetryBackend.hpp"

#include <format>
#include <iterator>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/proto/Translation2dProto.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ProtobufTopic.hpp"
#include "wpi/nt/StructArrayTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryEntry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace {
struct ThrowingTelemetryStruct {
  int16_t value;
};

struct ThrowingTelemetryStructState {
  static inline bool throwDuringPack = false;
};

struct ThrowingTelemetryProto {
  double x;
  double y;
};

struct ThrowingTelemetryProtoState {
  static inline bool throwDuringPack = false;
};

bool HasWarning(const std::vector<std::string>& warnings, std::string_view path,
                std::string_view msg) {
  for (const auto& warning : warnings) {
    if (warning.find(path) != std::string::npos &&
        warning.find(msg) != std::string::npos) {
      return true;
    }
  }
  return false;
}
}  // namespace

template <>
struct wpi::util::Struct<ThrowingTelemetryStruct> {
  static constexpr std::string_view GetTypeName() {
    return "ThrowingTelemetryStruct";
  }
  static constexpr size_t GetSize() { return 2; }
  static constexpr std::string_view GetSchema() { return "int16 value"; }
  static ThrowingTelemetryStruct Unpack(std::span<const uint8_t> data) {
    return {wpi::util::UnpackStruct<int16_t, 0>(data)};
  }
  static void Pack(std::span<uint8_t> data, ThrowingTelemetryStruct value) {
    if (ThrowingTelemetryStructState::throwDuringPack) {
      throw std::runtime_error{"broken struct pack"};
    }
    wpi::util::PackStruct<0>(data, value.value);
  }
};

template <>
struct wpi::util::Protobuf<ThrowingTelemetryProto> {
  using MessageStruct = wpi_proto_ProtobufTranslation2d;
  using InputStream = wpi::util::ProtoInputStream<ThrowingTelemetryProto>;
  using OutputStream = wpi::util::ProtoOutputStream<ThrowingTelemetryProto>;

  static std::optional<ThrowingTelemetryProto> Unpack(InputStream& stream) {
    wpi_proto_ProtobufTranslation2d msg{};
    if (!stream.Decode(msg)) {
      return {};
    }
    return ThrowingTelemetryProto{msg.x, msg.y};
  }

  static bool Pack(OutputStream& stream, const ThrowingTelemetryProto& value) {
    if (ThrowingTelemetryProtoState::throwDuringPack) {
      throw std::runtime_error{"broken protobuf pack"};
    }
    wpi_proto_ProtobufTranslation2d msg{.x = value.x, .y = value.y};
    return stream.Encode(msg);
  }
};

class NetworkTablesTelemetryBackendTest {
 public:
  NetworkTablesTelemetryBackendTest()
      : inst{wpi::nt::NetworkTableInstance::Create()},
        backend{std::make_shared<wpi::backend::NetworkTablesTelemetryBackend>(
            inst, "/Telemetry")} {
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::telemetry::TelemetryRegistry::RegisterBackend("", backend);
  }

  ~NetworkTablesTelemetryBackendTest() {
    wpi::telemetry::TelemetryRegistry::SetReportWarning(nullptr);
    wpi::telemetry::TelemetryRegistry::Reset();
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst;
  std::shared_ptr<wpi::backend::NetworkTablesTelemetryBackend> backend;
};

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest PublishesScalarDataTypes",
                 "[wpilibc][telemetry]") {
  auto boolean = inst.GetTopic("/Telemetry/boolean").GenericSubscribe();
  auto byte = inst.GetTopic("/Telemetry/byte").GenericSubscribe();
  auto shortValue = inst.GetTopic("/Telemetry/short").GenericSubscribe();
  auto intValue = inst.GetTopic("/Telemetry/int").GenericSubscribe();
  auto longValue = inst.GetTopic("/Telemetry/long").GenericSubscribe();
  auto floatValue = inst.GetTopic("/Telemetry/float").GenericSubscribe();
  auto doubleValue = inst.GetTopic("/Telemetry/double").GenericSubscribe();
  auto stringValue = inst.GetTopic("/Telemetry/string").GenericSubscribe();
  auto json = inst.GetTopic("/Telemetry/json").GenericSubscribe();

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

  CHECK(boolean.GetBoolean(false));
  CHECK(2 == byte.GetInteger(0));
  CHECK(3 == shortValue.GetInteger(0));
  CHECK(4 == intValue.GetInteger(0));
  CHECK(5 == longValue.GetInteger(0));
  CHECK(6.25f == floatValue.GetFloat(0.0f));
  CHECK(7.5 == doubleValue.GetDouble(0.0));
  CHECK("ready" == stringValue.GetString(""));
  CHECK("json" == inst.GetTopic("/Telemetry/json").GetTypeString());
  CHECK("{\"ok\":true}" == json.GetString(""));
}

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest PublishesExplicitTimestamp",
                 "[wpilibc][telemetry]") {
  constexpr int64_t timestamp = 123456789;
  auto sub = inst.GetDoubleTopic("/Telemetry/timestamped").Subscribe(0.0);

  backend->GetEntry("/timestamped")->LogDouble(2.5, timestamp);

  auto value = sub.GetAtomic();
  CHECK(timestamp == value.time);
  CHECK(2.5 == value.value);
}

TEST_CASE_METHOD(
    NetworkTablesTelemetryBackendTest,
    "NetworkTablesTelemetryBackendTest PublishesArrayAndRawDataTypes",
    "[wpilibc][telemetry]") {
  auto booleans = inst.GetTopic("/Telemetry/booleans").GenericSubscribe();
  auto shorts = inst.GetTopic("/Telemetry/shorts").GenericSubscribe();
  auto ints = inst.GetTopic("/Telemetry/ints").GenericSubscribe();
  auto longs = inst.GetTopic("/Telemetry/longs").GenericSubscribe();
  auto floats = inst.GetTopic("/Telemetry/floats").GenericSubscribe();
  auto doubles = inst.GetTopic("/Telemetry/doubles").GenericSubscribe();
  auto strings = inst.GetTopic("/Telemetry/strings").GenericSubscribe();
  auto stringViews = inst.GetTopic("/Telemetry/stringViews").GenericSubscribe();
  auto raw = inst.GetTopic("/Telemetry/raw").GenericSubscribe();

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

  auto customRaw = inst.GetTopic("/Telemetry/customRaw").GenericSubscribe();
  CHECK((std::vector<int>{1, 0}) == booleans.GetBooleanArray({}));
  CHECK((std::vector<int64_t>{1, 2}) == shorts.GetIntegerArray({}));
  CHECK((std::vector<int64_t>{3, 4}) == ints.GetIntegerArray({}));
  CHECK((std::vector<int64_t>{5, 6}) == longs.GetIntegerArray({}));
  CHECK((std::vector<float>{7.25f, 8.5f}) == floats.GetFloatArray({}));
  CHECK((std::vector<double>{9.25, 10.5}) == doubles.GetDoubleArray({}));
  CHECK((std::vector<std::string>{"a", "b"}) == strings.GetStringArray({}));
  CHECK((std::vector<std::string>{"c", "d"}) == stringViews.GetStringArray({}));
  CHECK((std::vector<uint8_t>{11, 12, 13}) == raw.GetRaw({}));
  CHECK("custom" == inst.GetTopic("/Telemetry/customRaw").GetTypeString());
  CHECK((std::vector<uint8_t>{14, 15}) == customRaw.GetRaw({}));
}

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest PublishesStruct",
                 "[wpilibc][telemetry]") {
  const wpi::math::Translation2d value{1.25_m, 2.5_m};
  auto sub =
      inst.GetStructTopic<wpi::math::Translation2d>("/Telemetry/translation")
          .Subscribe({});

  wpi::telemetry::Log("translation", value);

  auto logged = sub.Get();
  CHECK(value.X() == logged.X());
  CHECK(value.Y() == logged.Y());
}

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest PublishesStructArray",
                 "[wpilibc][telemetry]") {
  const wpi::math::Translation2d values[] = {{1.25_m, 2.5_m}, {3.75_m, 4.5_m}};
  auto sub = inst.GetStructArrayTopic<wpi::math::Translation2d>(
                     "/Telemetry/translations")
                 .Subscribe({});

  wpi::telemetry::Log("translations",
                      std::span<const wpi::math::Translation2d>{values});

  auto logged = sub.Get();
  REQUIRE(2u == logged.size());
  CHECK(values[0].X() == logged[0].X());
  CHECK(values[0].Y() == logged[0].Y());
  CHECK(values[1].X() == logged[1].X());
  CHECK(values[1].Y() == logged[1].Y());
}

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest PublishesProtobuf",
                 "[wpilibc][telemetry]") {
  const wpi::math::Translation2d value{3.75_m, 4.5_m};
  auto sub =
      inst.GetProtobufTopic<wpi::math::Translation2d>("/Telemetry/translation")
          .Subscribe({});
  wpi::util::ProtobufMessage<wpi::math::Translation2d> message;
  wpi::util::SmallVector<uint8_t, 128> buf;
  REQUIRE(message.Pack(buf, value));

  wpi::telemetry::Log("translation", std::span<const uint8_t>{buf},
                      message.GetTypeString());

  auto logged = sub.Get();
  CHECK(value.X() == logged.X());
  CHECK(value.Y() == logged.Y());
}

TEST_CASE_METHOD(
    NetworkTablesTelemetryBackendTest,
    "NetworkTablesTelemetryBackendTest StructuredSerializationFailuresWarn",
    "[wpilibc][telemetry]") {
  const ThrowingTelemetryStruct initialStruct{1};
  const ThrowingTelemetryStruct updatedStruct{2};
  const ThrowingTelemetryStruct initialStructs[] = {{3}, {4}};
  const ThrowingTelemetryStruct updatedStructs[] = {{5}, {6}};
  const ThrowingTelemetryProto initialProto{7.25, 8.5};
  const ThrowingTelemetryProto updatedProto{9.25, 10.5};
  auto structSub =
      inst.GetStructTopic<ThrowingTelemetryStruct>("/Telemetry/throwingStruct")
          .Subscribe({});
  auto structArraySub = inst.GetStructArrayTopic<ThrowingTelemetryStruct>(
                                "/Telemetry/throwingStructs")
                            .Subscribe({});
  auto protoSub =
      inst.GetProtobufTopic<ThrowingTelemetryProto>("/Telemetry/throwingProto")
          .Subscribe({});

  wpi::telemetry::Log("throwingStruct", initialStruct);
  wpi::telemetry::Log("throwingStructs",
                      std::span<const ThrowingTelemetryStruct>{
                          initialStructs, std::size(initialStructs)});
  wpi::telemetry::Log("throwingProto", initialProto);

  CHECK(initialStruct.value == structSub.Get().value);
  auto loggedStructs = structArraySub.Get();
  REQUIRE(2u == loggedStructs.size());
  CHECK(initialStructs[0].value == loggedStructs[0].value);
  CHECK(initialStructs[1].value == loggedStructs[1].value);
  auto loggedProto = protoSub.Get();
  CHECK(initialProto.x == loggedProto.x);
  CHECK(initialProto.y == loggedProto.y);

  std::vector<std::string> warnings;
  wpi::telemetry::TelemetryRegistry::SetReportWarning(
      [&](std::string_view path, std::string_view msg) {
        warnings.emplace_back(std::format("{}: {}", path, msg));
      });
  ThrowingTelemetryStructState::throwDuringPack = true;
  ThrowingTelemetryProtoState::throwDuringPack = true;

  CHECK_NOTHROW(wpi::telemetry::Log("throwingStruct", updatedStruct));
  CHECK_NOTHROW(wpi::telemetry::Log(
      "throwingStructs", std::span<const ThrowingTelemetryStruct>{
                             updatedStructs, std::size(updatedStructs)}));
  CHECK_NOTHROW(wpi::telemetry::Log("throwingProto", updatedProto));

  ThrowingTelemetryStructState::throwDuringPack = false;
  ThrowingTelemetryProtoState::throwDuringPack = false;

  CHECK(initialStruct.value == structSub.Get().value);
  loggedStructs = structArraySub.Get();
  REQUIRE(2u == loggedStructs.size());
  CHECK(initialStructs[0].value == loggedStructs[0].value);
  CHECK(initialStructs[1].value == loggedStructs[1].value);
  loggedProto = protoSub.Get();
  CHECK(initialProto.x == loggedProto.x);
  CHECK(initialProto.y == loggedProto.y);
  CHECK(HasWarning(warnings, "/throwingStruct",
                   "failed to publish struct value"));
  CHECK(HasWarning(warnings, "/throwingStructs",
                   "failed to publish struct array value"));
  CHECK(HasWarning(warnings, "/throwingProto",
                   "failed to publish protobuf value"));
}

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest AppliesTelemetryProperties",
                 "[wpilibc][telemetry]") {
  wpi::telemetry::SetProperty("speed", "min", "0");
  wpi::telemetry::SetProperty("speed", "unit", "\"m/s\"");

  wpi::telemetry::Log("speed", 4.0);
  wpi::telemetry::SetProperty("speed", "max", "10");

  auto topic = inst.GetTopic("/Telemetry/speed");
  auto min = topic.GetProperty("min");
  auto max = topic.GetProperty("max");
  auto unit = topic.GetProperty("unit");
  REQUIRE(min.is_number());
  REQUIRE(max.is_number());
  REQUIRE(unit.is_string());
  CHECK(0.0 == min.get_number());
  CHECK(10.0 == max.get_number());
  CHECK("m/s" == unit.get_string());
}

TEST_CASE_METHOD(
    NetworkTablesTelemetryBackendTest,
    "NetworkTablesTelemetryBackendTest KeepDuplicatesAfterPublishing",
    "[wpilibc][telemetry]") {
  auto sub = inst.GetDoubleTopic("/Telemetry/duplicates")
                 .Subscribe(0.0, {.pollStorage = 10, .keepDuplicates = true});

  wpi::telemetry::Log("duplicates", 1.0);
  wpi::telemetry::KeepDuplicates("duplicates");
  CHECK(1.0 == sub.Get());

  wpi::telemetry::Log("duplicates", 1.0);

  auto values = sub.ReadQueue();
  REQUIRE(2u == values.size());
  CHECK(1.0 == values[0].value);
  CHECK(1.0 == values[1].value);
}

TEST_CASE_METHOD(
    NetworkTablesTelemetryBackendTest,
    "NetworkTablesTelemetryBackendTest RemovedEntryDoesNotRecreatePublisher",
    "[wpilibc][telemetry]") {
  auto staleEntry = backend->GetEntry("/stale");
  backend->RemoveEntry("/stale");

  staleEntry->LogDouble(1.25, 0);

  auto topic = inst.GetTopic("/Telemetry/stale");
  CHECK_FALSE(topic.Exists());

  auto sub = topic.GenericSubscribe();
  backend->GetEntry("/stale")->LogDouble(2.5, 0);

  CHECK(topic.Exists());
  CHECK(2.5 == sub.GetDouble(0.0));
}

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest "
                 "TypeMismatchWarningAllowsSameEntryReentry",
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
