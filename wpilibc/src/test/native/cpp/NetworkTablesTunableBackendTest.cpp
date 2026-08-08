// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/NetworkTablesTunableBackend.hpp"

#include <format>
#include <memory>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ProtobufTopic.hpp"
#include "wpi/nt/StringArrayTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/Selectable.hpp"
#include "wpi/tunable/Tunable.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/TunableTable.hpp"
#include "wpi/tunable/Tunables.hpp"

namespace {

class MutatingComplexTunable final : public wpi::ComplexTunable {
 public:
  explicit MutatingComplexTunable(wpi::TunableDouble& published)
      : m_published{published} {}

  std::string_view GetTunableType() const override { return "Mutating"; }

  void PublishTunable(wpi::TunableTable&) override {}

  void UpdateTunable() const override {
    if (m_updates++ != 1) {
      return;
    }
    wpi::Tunables::Remove("removeMe");
    wpi::Tunables::Publish("publishedFromComplex", m_published);
  }

  int GetUpdates() const { return m_updates; }

 private:
  wpi::TunableDouble& m_published;
  mutable int m_updates = 0;
};

class CountingComplexTunable final : public wpi::ComplexTunable {
 public:
  std::string_view GetTunableType() const override { return "Counting"; }

  void PublishTunable(wpi::TunableTable&) override {}

  void UpdateTunable() const override { ++m_updates; }

  int GetUpdates() const { return m_updates; }

 private:
  mutable int m_updates = 0;
};

}  // namespace

class NetworkTablesTunableBackendTest {
 public:
  NetworkTablesTunableBackendTest()
      : inst{wpi::nt::NetworkTableInstance::Create()},
        backend{std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
            inst, "/Tunables")} {
    wpi::TunableRegistry::Reset();
    wpi::TunableRegistry::RegisterBackend("", backend);
  }

  ~NetworkTablesTunableBackendTest() {
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
    return inst.GetTopic(std::format("/Tunables/{}/value", name))
        .GenericSubscribe();
  }

  wpi::nt::GenericPublisher Tune(std::string_view name,
                                 std::string_view typeString) {
    return inst.GetTopic(std::format("/Tunables/{}/tune", name))
        .GenericPublish(typeString);
  }
};

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest PublishesAndTunesScalarDataTypes",
    "[wpilibc][tunable]") {
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

  CHECK(Value("boolean").GetBoolean(false));
  CHECK(1 == Value("int").GetInteger(0));
  CHECK(2 == Value("long").GetInteger(0));
  CHECK(3.25f == Value("float").GetFloat(0.0f));
  CHECK(4.5 == Value("double").GetDouble(0.0));
  CHECK("ready" == Value("string").GetString(""));

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

  CHECK_FALSE(boolean.Get());
  CHECK(11 == intValue.Get());
  CHECK(12 == longValue.Get());
  CHECK(13.5f == floatValue.Get());
  CHECK(14.75 == doubleValue.Get());
  CHECK("tuned" == stringValue.Get());
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest PublishesAndTunesArrayAndRawDataTypes",
    "[wpilibc][tunable]") {
  auto config = RobustConfig();
  wpi::TunableRaw raw{std::vector<uint8_t>{1, 2}, config};
  wpi::TunableBoolVector booleans{std::vector<bool>{true, false}, config};
  wpi::TunableInt32Vector ints{std::vector<int32_t>{3, 4}, config};
  wpi::TunableInt64Vector longs{std::vector<int64_t>{5, 6}, config};
  wpi::TunableFloatVector floats{std::vector<float>{7.25f, 8.5f}, config};
  wpi::TunableDoubleVector doubles{std::vector<double>{9.25, 10.5}, config};
  wpi::Tunable<std::vector<std::string>> strings{
      std::vector<std::string>{"a", "b"}, config};

  wpi::Tunables::Publish("raw", raw);
  wpi::Tunables::Publish("booleans", booleans);
  wpi::Tunables::Publish("ints", ints);
  wpi::Tunables::Publish("longs", longs);
  wpi::Tunables::Publish("floats", floats);
  wpi::Tunables::Publish("doubles", doubles);
  wpi::Tunables::Publish("strings", strings);

  CHECK((std::vector<uint8_t>{1, 2}) == Value("raw").GetRaw({}));
  CHECK((std::vector<int>{1, 0}) == Value("booleans").GetBooleanArray({}));
  CHECK((std::vector<int64_t>{3, 4}) == Value("ints").GetIntegerArray({}));
  CHECK((std::vector<int64_t>{5, 6}) == Value("longs").GetIntegerArray({}));
  CHECK((std::vector<float>{7.25f, 8.5f}) == Value("floats").GetFloatArray({}));
  CHECK((std::vector<double>{9.25, 10.5}) ==
        Value("doubles").GetDoubleArray({}));
  CHECK((std::vector<std::string>{"a", "b"}) ==
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

  CHECK((std::vector<uint8_t>{21, 22}) == raw.Get());
  CHECK((std::vector<bool>{false, true}) == booleans.Get());
  CHECK((std::vector<int32_t>{23, 24}) == ints.Get());
  CHECK((std::vector<int64_t>{25, 26}) == longs.Get());
  CHECK((std::vector<float>{27.25f, 28.5f}) == floats.Get());
  CHECK((std::vector<double>{29.25, 30.5}) == doubles.Get());
  CHECK((std::vector<std::string>{"c", "d"}) == strings.Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest PublishesAndTunesDouble",
                 "[wpilibc][tunable]") {
  wpi::TunableConfig config;
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo").Subscribe(0.0);
  CHECK(sub.Get() == 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  CHECK(value.Get() == 2.0);
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest TunablesWithoutConfigAreMutable",
    "[wpilibc][tunable]") {
  wpi::TunableDouble value{1.0};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo").Subscribe(0.0);
  CHECK(sub.Get() == 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  CHECK(value.Get() == 2.0);
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest PublishesRobustDouble",
                 "[wpilibc][tunable]") {
  wpi::TunableConfig config;
  config.robust = true;
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo/value").Subscribe(0.0);
  CHECK(sub.Get() == 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo/tune").Publish();
  pub.Set(2.0);
  wpi::TunableRegistry::Update();

  CHECK(value.Get() == 2.0);
  CHECK(sub.Get() == 2.0);
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "SelectablePublishesOptionChangesAfterPublish",
                 "[wpilibc][tunable]") {
  wpi::Selectable<int> chooser;
  chooser.Add("one", 1);
  wpi::Tunables::Publish("chooser", chooser);

  auto sub =
      inst.GetStringArrayTopic("/Tunables/chooser/options").Subscribe({});
  CHECK((std::vector<std::string>{"one"}) == sub.Get());

  chooser.Add("two", 2);
  wpi::TunableRegistry::Update();

  CHECK((std::vector<std::string>{"one", "two"}) == sub.Get());

  chooser.Add("one", 11);
  wpi::TunableRegistry::Update();

  CHECK((std::vector<std::string>{"two", "one"}) == sub.Get());

  chooser.Clear();
  wpi::TunableRegistry::Update();

  CHECK((std::vector<std::string>{}) == sub.Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest PublishesAndTunesStruct",
                 "[wpilibc][tunable]") {
  const wpi::math::Translation2d initial{1.25_m, 2.5_m};
  const wpi::math::Translation2d tuned{3.75_m, 4.5_m};
  wpi::TunableConfig config;
  config.robust = true;
  wpi::Tunable<wpi::math::Translation2d> value{config, initial};
  wpi::Tunables::Publish("translation", value);

  auto sub = inst.GetStructTopic<wpi::math::Translation2d>(
                     "/Tunables/translation/value")
                 .Subscribe({});
  auto logged = sub.Get();
  CHECK(initial.X() == logged.X());
  CHECK(initial.Y() == logged.Y());

  auto pub = inst.GetStructTopic<wpi::math::Translation2d>(
                     "/Tunables/translation/tune")
                 .Publish();
  pub.Set(tuned);
  inst.Flush();
  wpi::TunableRegistry::Update();

  CHECK(tuned.X() == value.Get().X());
  CHECK(tuned.Y() == value.Get().Y());
  logged = sub.Get();
  CHECK(tuned.X() == logged.X());
  CHECK(tuned.Y() == logged.Y());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest PublishesAndTunesProtobuf",
                 "[wpilibc][tunable]") {
  const wpi::math::Translation2d initial{5.25_m, 6.5_m};
  const wpi::math::Translation2d tuned{7.75_m, 8.5_m};
  wpi::TunableConfig config;
  config.robust = true;
  wpi::detail::TunableProtobuf<wpi::math::Translation2d> value{config, initial};
  wpi::Tunables::Publish("translation", value);

  auto sub = inst.GetProtobufTopic<wpi::math::Translation2d>(
                     "/Tunables/translation/value")
                 .Subscribe({});
  auto logged = sub.Get();
  CHECK(initial.X() == logged.X());
  CHECK(initial.Y() == logged.Y());

  auto pub = inst.GetProtobufTopic<wpi::math::Translation2d>(
                     "/Tunables/translation/tune")
                 .Publish();
  pub.Set(tuned);
  inst.Flush();
  wpi::TunableRegistry::Update();

  CHECK(tuned.X() == value.Get().X());
  CHECK(tuned.Y() == value.Get().Y());
  logged = sub.Get();
  CHECK(tuned.X() == logged.X());
  CHECK(tuned.Y() == logged.Y());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest AppliesBackendConfigOptions",
                 "[wpilibc][tunable]") {
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
  CHECK("json" == topic.GetTypeString());
  CHECK(true == topic.GetProperty("robust"));
  CHECK(0 == topic.GetProperty("min"));
  CHECK(10 == topic.GetProperty("max"));
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest OnTuneRunsForMutableRemoteUpdates",
    "[wpilibc][tunable]") {
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

  CHECK(2.0 == value.Get());
  CHECK(1 == calls);
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest OnTuneCanPublishAndRemoveTunables",
    "[wpilibc][tunable]") {
  bool callbackRan = false;
  wpi::TunableDouble published{3.0};
  wpi::TunableDouble removeMe{4.0};
  wpi::Tunables::Publish("removeMe", removeMe);

  wpi::TunableConfig config;
  config.robust = true;
  config.onTune = [&](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
    if (callbackRan) {
      return;
    }
    callbackRan = true;
    wpi::Tunables::Remove("removeMe");
    wpi::Tunables::Publish("publishedFromOnTune", published);
  };
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("mutable", value);

  auto pub = Tune("mutable", "double");
  pub.SetDouble(2.0);
  inst.Flush();
  wpi::TunableRegistry::Update();

  CHECK(callbackRan);
  CHECK(2.0 == value.Get());
  CHECK(3.0 == inst.GetDoubleTopic("/Tunables/publishedFromOnTune")
                   .Subscribe(0.0)
                   .Get());

  wpi::TunableDouble replacement{5.0};
  CHECK_NOTHROW(wpi::Tunables::Publish("removeMe", replacement));
  CHECK(5.0 == inst.GetDoubleTopic("/Tunables/removeMe").Subscribe(0.0).Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "ComplexUpdateCanPublishAndRemoveTunables",
                 "[wpilibc][tunable]") {
  wpi::TunableDouble published{3.0};
  wpi::TunableDouble removeMe{4.0};
  wpi::Tunables::Publish("removeMe", removeMe);

  MutatingComplexTunable complex{published};
  wpi::Tunables::Publish("complex", complex);
  CountingComplexTunable after;
  wpi::Tunables::Publish("z", after);

  wpi::TunableRegistry::Update();

  CHECK(2 == complex.GetUpdates());
  CHECK(2 == after.GetUpdates());
  CHECK(3.0 == inst.GetDoubleTopic("/Tunables/publishedFromComplex")
                   .Subscribe(0.0)
                   .Get());

  wpi::TunableDouble replacement{5.0};
  CHECK_NOTHROW(wpi::Tunables::Publish("removeMe", replacement));
  CHECK(5.0 == inst.GetDoubleTopic("/Tunables/removeMe").Subscribe(0.0).Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "NonRobustTunablesDoNotTuneFromLocalPublishes",
                 "[wpilibc][tunable]") {
  int calls = 0;
  wpi::TunableConfig config;
  config.onTune = [&](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
    ++calls;
  };
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("localPublish", value);

  inst.Flush();
  wpi::TunableRegistry::Update();

  CHECK(0 == calls);

  value = 2.0;
  wpi::TunableRegistry::Update();
  inst.Flush();
  wpi::TunableRegistry::Update();

  auto sub = inst.GetDoubleTopic("/Tunables/localPublish").Subscribe(0.0);
  CHECK(2.0 == value.Get());
  CHECK(2.0 == sub.Get());
  CHECK(0 == calls);
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest ImmutableTunablesIgnoreRemoteUpdates",
    "[wpilibc][tunable]") {
  int calls = 0;
  wpi::TunableConfig config;
  config.robust = true;
  config.isMutable = false;
  config.onTune = [&](wpi::detail::TunableBase&, wpi::ComplexTunable*) {
    ++calls;
  };
  wpi::TunableDouble value{1.0, config};
  wpi::Tunables::Publish("immutable", value);

  CHECK(false ==
        inst.GetTopic("/Tunables/immutable/value").GetProperty("mutable"));

  auto pub = Tune("immutable", "double");
  pub.SetDouble(2.0);
  inst.Flush();
  wpi::TunableRegistry::Update();

  CHECK(1.0 == value.Get());
  CHECK(0 == calls);
}
