// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <memory>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "wpi/backend/NetworkTablesTunableBackend.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ProtobufTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/tunable/Tunable.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

class NetworkTablesTunableBackendTest : public ::testing::Test {
 public:
  NetworkTablesTunableBackendTest()
      : inst{wpi::nt::NetworkTableInstance::Create()},
        backend{std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
            inst, "/Tunables")} {
    wpi::TunableRegistry::Reset();
    wpi::TunableRegistry::RegisterBackend("", backend);
  }

  ~NetworkTablesTunableBackendTest() override {
    wpi::TunableRegistry::Reset();
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst;
  std::shared_ptr<wpi::backend::NetworkTablesTunableBackend> backend;

 protected:
  static wpi::TunableConfig RobustConfig() {
    wpi::TunableConfig config;
    config.robust = true;
    return config;
  }

  wpi::nt::GenericSubscriber Value(std::string_view name) {
    return inst.GetTopic(fmt::format("/Tunables/{}/value", name))
        .GenericSubscribe();
  }

  wpi::nt::GenericPublisher Tune(std::string_view name,
                                 std::string_view typeString) {
    return inst.GetTopic(fmt::format("/Tunables/{}/tune", name))
        .GenericPublish(typeString);
  }
};

TEST_F(NetworkTablesTunableBackendTest, PublishesAndTunesScalarDataTypes) {
  auto config = RobustConfig();
  wpi::TunableBool boolean{true, config};
  wpi::TunableInt32 intValue{1, config};
  wpi::TunableInt64 longValue{2, config};
  wpi::TunableFloat floatValue{3.25f, config};
  wpi::TunableDouble doubleValue{4.5, config};
  wpi::TunableString stringValue{"ready", config};

  wpi::Tunables::Publish("boolean", boolean);
  wpi::Tunables::Publish("int", intValue);
  wpi::Tunables::Publish("long", longValue);
  wpi::Tunables::Publish("float", floatValue);
  wpi::Tunables::Publish("double", doubleValue);
  wpi::Tunables::Publish("string", stringValue);

  EXPECT_TRUE(Value("boolean").GetBoolean(false));
  EXPECT_EQ(1, Value("int").GetInteger(0));
  EXPECT_EQ(2, Value("long").GetInteger(0));
  EXPECT_EQ(3.25f, Value("float").GetFloat(0.0f));
  EXPECT_EQ(4.5, Value("double").GetDouble(0.0));
  EXPECT_EQ("ready", Value("string").GetString(""));

  auto booleanPub = Tune("boolean", "boolean");
  auto intPub = Tune("int", "int");
  auto longPub = Tune("long", "int");
  auto floatPub = Tune("float", "float");
  auto doublePub = Tune("double", "double");
  auto stringPub = Tune("string", "string");
  booleanPub.SetBoolean(false);
  intPub.SetInteger(11);
  longPub.SetInteger(12);
  floatPub.SetFloat(13.5f);
  doublePub.SetDouble(14.75);
  stringPub.SetString("tuned");
  inst.Flush();
  wpi::TunableRegistry::Update();

  EXPECT_FALSE(boolean.Get());
  EXPECT_EQ(11, intValue.Get());
  EXPECT_EQ(12, longValue.Get());
  EXPECT_EQ(13.5f, floatValue.Get());
  EXPECT_EQ(14.75, doubleValue.Get());
  EXPECT_EQ("tuned", stringValue.Get());
}

TEST_F(NetworkTablesTunableBackendTest, PublishesAndTunesArrayAndRawDataTypes) {
  auto config = RobustConfig();
  wpi::TunableRaw raw{std::vector<uint8_t>{1, 2}, config};
  wpi::TunableBoolVector booleans{std::vector<bool>{true, false}, config};
  wpi::TunableInt32Vector ints{std::vector<int32_t>{3, 4}, config};
  wpi::TunableInt64Vector longs{std::vector<int64_t>{5, 6}, config};
  wpi::TunableFloatVector floats{std::vector<float>{7.25f, 8.5f}, config};
  wpi::TunableDoubleVector doubles{std::vector<double>{9.25, 10.5},
                                  config};
  wpi::Tunable<std::vector<std::string>> strings{
      std::vector<std::string>{"a", "b"}, config};

  wpi::Tunables::Publish("raw", raw);
  wpi::Tunables::Publish("booleans", booleans);
  wpi::Tunables::Publish("ints", ints);
  wpi::Tunables::Publish("longs", longs);
  wpi::Tunables::Publish("floats", floats);
  wpi::Tunables::Publish("doubles", doubles);
  wpi::Tunables::Publish("strings", strings);

  EXPECT_EQ((std::vector<uint8_t>{1, 2}), Value("raw").GetRaw({}));
  EXPECT_EQ((std::vector<int>{1, 0}), Value("booleans").GetBooleanArray({}));
  EXPECT_EQ((std::vector<int64_t>{3, 4}), Value("ints").GetIntegerArray({}));
  EXPECT_EQ((std::vector<int64_t>{5, 6}), Value("longs").GetIntegerArray({}));
  EXPECT_EQ((std::vector<float>{7.25f, 8.5f}),
            Value("floats").GetFloatArray({}));
  EXPECT_EQ((std::vector<double>{9.25, 10.5}),
            Value("doubles").GetDoubleArray({}));
  EXPECT_EQ((std::vector<std::string>{"a", "b"}),
            Value("strings").GetStringArray({}));

  auto rawPub = Tune("raw", "raw");
  auto booleansPub = Tune("booleans", "boolean[]");
  auto intsPub = Tune("ints", "int[]");
  auto longsPub = Tune("longs", "int[]");
  auto floatsPub = Tune("floats", "float[]");
  auto doublesPub = Tune("doubles", "double[]");
  auto stringsPub = Tune("strings", "string[]");
  rawPub.SetRaw(std::vector<uint8_t>{21, 22});
  booleansPub.SetBooleanArray(std::vector<int>{0, 1});
  intsPub.SetIntegerArray(std::vector<int64_t>{23, 24});
  longsPub.SetIntegerArray(std::vector<int64_t>{25, 26});
  floatsPub.SetFloatArray(std::vector<float>{27.25f, 28.5f});
  doublesPub.SetDoubleArray(std::vector<double>{29.25, 30.5});
  stringsPub.SetStringArray(std::vector<std::string>{"c", "d"});
  inst.Flush();
  wpi::TunableRegistry::Update();

  EXPECT_EQ((std::vector<uint8_t>{21, 22}), raw.Get());
  EXPECT_EQ((std::vector<bool>{false, true}), booleans.Get());
  EXPECT_EQ((std::vector<int32_t>{23, 24}), ints.Get());
  EXPECT_EQ((std::vector<int64_t>{25, 26}), longs.Get());
  EXPECT_EQ((std::vector<float>{27.25f, 28.5f}), floats.Get());
  EXPECT_EQ((std::vector<double>{29.25, 30.5}), doubles.Get());
  EXPECT_EQ((std::vector<std::string>{"c", "d"}), strings.Get());
}

TEST_F(NetworkTablesTunableBackendTest, PublishesAndTunesDouble) {
  wpi::TunableConfig config;
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo").Subscribe(0.0);
  EXPECT_EQ(sub.Get(), 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  EXPECT_EQ(value.Get(), 2.0);
}

TEST_F(NetworkTablesTunableBackendTest, TunablesWithoutConfigAreMutable) {
  wpi::TunableDouble value{1.0};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo").Subscribe(0.0);
  EXPECT_EQ(sub.Get(), 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  EXPECT_EQ(value.Get(), 2.0);
}

TEST_F(NetworkTablesTunableBackendTest, PublishesRobustDouble) {
  wpi::TunableConfig config;
  config.robust = true;
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo/value").Subscribe(0.0);
  EXPECT_EQ(sub.Get(), 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo/tune").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  EXPECT_EQ(value.Get(), 2.0);
  EXPECT_EQ(sub.Get(), 2.0);
}

TEST_F(NetworkTablesTunableBackendTest, PublishesAndTunesStruct) {
  const wpi::math::Translation2d initial{1.25_m, 2.5_m};
  const wpi::math::Translation2d tuned{3.75_m, 4.5_m};
  wpi::TunableConfig config;
  config.robust = true;
  wpi::Tunable<wpi::math::Translation2d> value{config, initial};
  wpi::Tunables::Publish("translation", value);

  auto sub =
      inst.GetStructTopic<wpi::math::Translation2d>(
              "/Tunables/translation/value")
          .Subscribe({});
  auto logged = sub.Get();
  EXPECT_EQ(initial.X(), logged.X());
  EXPECT_EQ(initial.Y(), logged.Y());

  auto pub =
      inst.GetStructTopic<wpi::math::Translation2d>(
              "/Tunables/translation/tune")
          .Publish();
  pub.Set(tuned);
  inst.Flush();
  wpi::TunableRegistry::Update();

  EXPECT_EQ(tuned.X(), value.Get().X());
  EXPECT_EQ(tuned.Y(), value.Get().Y());
  logged = sub.Get();
  EXPECT_EQ(tuned.X(), logged.X());
  EXPECT_EQ(tuned.Y(), logged.Y());
}

TEST_F(NetworkTablesTunableBackendTest, PublishesAndTunesProtobuf) {
  const wpi::math::Translation2d initial{5.25_m, 6.5_m};
  const wpi::math::Translation2d tuned{7.75_m, 8.5_m};
  wpi::TunableConfig config;
  config.robust = true;
  wpi::detail::TunableProtobuf<wpi::math::Translation2d> value{config, initial};
  wpi::Tunables::Publish("translation", value);

  auto sub =
      inst.GetProtobufTopic<wpi::math::Translation2d>(
              "/Tunables/translation/value")
          .Subscribe({});
  auto logged = sub.Get();
  EXPECT_EQ(initial.X(), logged.X());
  EXPECT_EQ(initial.Y(), logged.Y());

  auto pub =
      inst.GetProtobufTopic<wpi::math::Translation2d>(
              "/Tunables/translation/tune")
          .Publish();
  pub.Set(tuned);
  inst.Flush();
  wpi::TunableRegistry::Update();

  EXPECT_EQ(tuned.X(), value.Get().X());
  EXPECT_EQ(tuned.Y(), value.Get().Y());
  logged = sub.Get();
  EXPECT_EQ(tuned.X(), logged.X());
  EXPECT_EQ(tuned.Y(), logged.Y());
}

TEST_F(NetworkTablesTunableBackendTest, AppliesBackendConfigOptions) {
  int calls = 0;
  wpi::TunableConfig config;
  config.robust = true;
  config.typeString = "json";
  config.properties = wpi::util::json::object();
  config.properties["min"] = 0;
  config.properties["max"] = 10;
  config.onTune = [&](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
    ++calls;
  };
  wpi::TunableString value{"1", config};
  wpi::Tunables::Publish("configured", value);

  auto topic = inst.GetTopic("/Tunables/configured/value");
  EXPECT_EQ("json", topic.GetTypeString());
  EXPECT_EQ(true, topic.GetProperty("robust"));
  EXPECT_EQ(0, topic.GetProperty("min"));
  EXPECT_EQ(10, topic.GetProperty("max"));
}

TEST_F(NetworkTablesTunableBackendTest, OnTuneRunsForMutableRemoteUpdates) {
  int calls = 0;
  wpi::TunableConfig config;
  config.robust = true;
  config.onTune = [&](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
    ++calls;
  };
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("mutable", value);

  auto pub = Tune("mutable", "double");
  pub.SetDouble(2.0);
  inst.Flush();
  wpi::TunableRegistry::Update();

  EXPECT_EQ(2.0, value.Get());
  EXPECT_EQ(1, calls);
}

TEST_F(NetworkTablesTunableBackendTest, ImmutableTunablesIgnoreRemoteUpdates) {
  int calls = 0;
  wpi::TunableConfig config;
  config.robust = true;
  config.isMutable = false;
  config.onTune = [&](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
    ++calls;
  };
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("immutable", value);

  EXPECT_EQ(false,
            inst.GetTopic("/Tunables/immutable/value").GetProperty("mutable"));

  auto pub = Tune("immutable", "double");
  pub.SetDouble(2.0);
  inst.Flush();
  wpi::TunableRegistry::Update();

  EXPECT_EQ(1.0, value.Get());
  EXPECT_EQ(0, calls);
}
