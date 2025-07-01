// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/NetworkTablesTelemetryBackend.hpp"

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ProtobufTopic.hpp"
#include "wpi/nt/StructArrayTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/telemetry/Telemetry.hpp"
#include "wpi/telemetry/TelemetryRegistry.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"

class NetworkTablesTelemetryBackendTest {
 public:
  NetworkTablesTelemetryBackendTest()
      : inst{wpi::nt::NetworkTableInstance::Create()},
        backend{std::make_shared<wpi::backend::NetworkTablesTelemetryBackend>(
            inst, "/Telemetry")} {
    wpi::TelemetryRegistry::Reset();
    wpi::TelemetryRegistry::RegisterBackend("", backend);
  }

  ~NetworkTablesTelemetryBackendTest() {
    wpi::TelemetryRegistry::Reset();
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

  wpi::Telemetry::Log("boolean", true);
  wpi::Telemetry::Log("byte", int8_t{2});
  wpi::Telemetry::Log("short", int16_t{3});
  wpi::Telemetry::Log("int", int32_t{4});
  wpi::Telemetry::Log("long", int64_t{5});
  wpi::Telemetry::Log("float", 6.25f);
  wpi::Telemetry::Log("double", 7.5);
  wpi::Telemetry::Log("string", "ready");
  wpi::Telemetry::Log("json", std::string_view{"{\"ok\":true}"},
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

  wpi::Telemetry::Log("booleans", boolValues);
  wpi::Telemetry::Log("shorts", shortValues);
  wpi::Telemetry::Log("ints", intValues);
  wpi::Telemetry::Log("longs", longValues);
  wpi::Telemetry::Log("floats", floatValues);
  wpi::Telemetry::Log("doubles", doubleValues);
  wpi::Telemetry::Log("strings", stringValues);
  wpi::Telemetry::Log("stringViews", stringViewValues);
  wpi::Telemetry::Log("raw", rawValues);
  wpi::Telemetry::Log("customRaw", std::span<const uint8_t>{customRawValues},
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

  wpi::Telemetry::Log("translation", value);

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

  wpi::Telemetry::Log("translations",
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

  wpi::Telemetry::Log("translation", std::span<const uint8_t>{buf},
                      message.GetTypeString());

  auto logged = sub.Get();
  CHECK(value.X() == logged.X());
  CHECK(value.Y() == logged.Y());
}

TEST_CASE_METHOD(NetworkTablesTelemetryBackendTest,
                 "NetworkTablesTelemetryBackendTest AppliesTelemetryProperties",
                 "[wpilibc][telemetry]") {
  wpi::Telemetry::SetProperty("speed", "min", "0");
  wpi::Telemetry::SetProperty("speed", "unit", "\"m/s\"");

  wpi::Telemetry::Log("speed", 4.0);
  wpi::Telemetry::SetProperty("speed", "max", "10");

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
