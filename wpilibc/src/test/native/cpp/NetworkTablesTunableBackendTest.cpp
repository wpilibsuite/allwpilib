// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/backend/NetworkTablesTunableBackend.hpp"

#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/GenericEntry.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ProtobufTopic.hpp"
#include "wpi/nt/StringArrayTopic.hpp"
#include "wpi/nt/StructArrayTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/tunables/ComplexTunable.hpp"
#include "wpi/tunables/Selectable.hpp"
#include "wpi/tunables/Tunable.hpp"
#include "wpi/tunables/TunableConfig.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/TunableTable.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/protobuf/Protobuf.hpp"
#include "wpi/util/struct/Struct.hpp"

struct ThrowingPackStruct {
  int16_t value;
};

struct ThrowingPackStructState {
  static inline bool removeDuringPack = false;
  static inline bool throwDuringPack = false;
  static inline bool throwDuringUnpack = false;
};

template <>
struct wpi::util::Struct<ThrowingPackStruct> {
  static constexpr std::string_view GetTypeName() {
    return "ThrowingPackStruct";
  }
  static constexpr size_t GetSize() { return 2; }
  static constexpr std::string_view GetSchema() { return "int16 value"; }
  static ThrowingPackStruct Unpack(std::span<const uint8_t> data) {
    if (ThrowingPackStructState::throwDuringUnpack) {
      ThrowingPackStructState::throwDuringUnpack = false;
      throw std::runtime_error{"broken unpack"};
    }
    return {wpi::util::UnpackStruct<int16_t, 0>(data)};
  }
  static void Pack(std::span<uint8_t> data, ThrowingPackStruct value) {
    if (ThrowingPackStructState::removeDuringPack) {
      ThrowingPackStructState::removeDuringPack = false;
      wpi::tunables::Remove("removeMe");
    }
    if (ThrowingPackStructState::throwDuringPack) {
      throw std::runtime_error{"broken pack"};
    }
    wpi::util::PackStruct<0>(data, value.value);
  }
};

namespace {

class MutatingComplexTunable final : public wpi::tunables::ComplexTunable {
 public:
  explicit MutatingComplexTunable(wpi::tunables::TunableDouble& published)
      : m_published{published} {}

  std::string_view GetTunableType() const override { return "Mutating"; }

  void PublishTunable(wpi::tunables::TunableTable&) override {}

  void UpdateTunable() const override {
    if (m_updates++ != 0) {
      return;
    }
    wpi::tunables::Remove("removeMe");
    wpi::tunables::Publish("publishedFromComplex", m_published);
  }

  int GetUpdates() const { return m_updates; }

 private:
  wpi::tunables::TunableDouble& m_published;
  mutable int m_updates = 0;
};

class CountingComplexTunable final : public wpi::tunables::ComplexTunable {
 public:
  std::string_view GetTunableType() const override { return "Counting"; }

  void PublishTunable(wpi::tunables::TunableTable&) override {}

  void UpdateTunable() const override { ++m_updates; }

  int GetUpdates() const { return m_updates; }

 private:
  mutable int m_updates = 0;
};

class StructMemberComplexTunable final : public wpi::tunables::ComplexTunable {
 public:
  explicit StructMemberComplexTunable(
      const wpi::tunables::TunableConfig& config)
      : m_config{config} {}

  std::string_view GetTunableType() const override { return "StructMember"; }

  void PublishTunable(wpi::tunables::TunableTable& table) override {
    table.Publish("member", this, &StructMemberComplexTunable::value, m_config);
  }

  ThrowingPackStruct value{1};

 private:
  wpi::tunables::TunableConfig m_config;
};

class DashboardSelectable {
 public:
  DashboardSelectable(wpi::nt::NetworkTableInstance inst, std::string_view path)
      : m_default{inst.GetTopic(std::format("{}/default", path))
                      .GenericSubscribe("string")},
        m_selected{inst.GetTopic(std::format("{}/selected/tune", path))
                       .GenericSubscribe("string")},
        m_selectedPub{inst.GetTopic(std::format("{}/selected/tune", path))
                          .GenericPublishEx("string", wpi::util::json::object(
                                                          "retained", true))},
        m_active{inst.GetTopic(std::format("{}/selected/value", path))
                     .GenericSubscribe("string")},
        m_options{inst.GetTopic(std::format("{}/options", path))
                      .GenericSubscribe("string[]")} {}

  bool Exists() const { return m_options.Exists(); }

  std::string GetDefault() const { return m_default.GetString(""); }

  std::string GetSelected() const { return m_selected.GetString(""); }

  std::string GetActive() const {
    std::string active = m_active.GetString("");
    return active.empty() ? GetDefault() : active;
  }

  std::vector<std::string> GetOptions() const {
    return m_options.GetStringArray(std::span<const std::string>{});
  }

  void SetSelected(std::string_view value) { m_selectedPub.SetString(value); }

 private:
  wpi::nt::GenericSubscriber m_default;
  wpi::nt::GenericSubscriber m_selected;
  wpi::nt::GenericPublisher m_selectedPub;
  wpi::nt::GenericSubscriber m_active;
  wpi::nt::GenericSubscriber m_options;
};

void ConfigureSelectable(wpi::tunables::Selectable<int>& chooser) {
  chooser.AddDefault("one", 1);
  chooser.Add("two", 2);
}

bool HasWarning(const std::vector<std::string>& warnings, std::string_view path,
                std::string_view msg) {
  for (auto&& warning : warnings) {
    if (warning.find(path) != std::string::npos &&
        warning.find(msg) != std::string::npos) {
      return true;
    }
  }
  return false;
}

}  // namespace

class NetworkTablesTunableBackendTest {
 public:
  NetworkTablesTunableBackendTest()
      : inst{wpi::nt::NetworkTableInstance::Create()},
        backend{std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
            inst, "/Tunables")} {
    wpi::tunables::TunableRegistry::Reset();
    wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
    wpi::tunables::TunableRegistry::RegisterBackend("", backend);
  }

  ~NetworkTablesTunableBackendTest() {
    wpi::tunables::TunableRegistry::Reset();
    wpi::tunables::TunableRegistry::SetReportWarning(nullptr);
    wpi::nt::NetworkTableInstance::Destroy(inst);
  }

  wpi::nt::NetworkTableInstance inst;
  std::shared_ptr<wpi::backend::NetworkTablesTunableBackend> backend;

 protected:
  void RestartRobotBackend() {
    wpi::tunables::TunableRegistry::Reset();
    backend.reset();
    backend = std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
        inst, "/Tunables");
    wpi::tunables::TunableRegistry::RegisterBackend("", backend);
  }

  static wpi::tunables::TunableConfig RobustConfig() {
    wpi::tunables::TunableConfig config;
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

namespace wpi::backend {
class NetworkTablesTunableBackendTestAccess {
 public:
  static int GetUpdateDepth(const NetworkTablesTunableBackend& backend) {
    return backend.m_updateDepth;
  }

  static size_t GetEntryCount(const NetworkTablesTunableBackend& backend) {
    return backend.m_entries.size();
  }

  static size_t GetUidPathCount(const NetworkTablesTunableBackend& backend) {
    size_t count = 0;
    for (const auto& entry : backend.m_uids) {
      count += entry.second.size();
    }
    return count;
  }

  static size_t GetDeferredEraseCount(
      const NetworkTablesTunableBackend& backend) {
    return backend.m_deferredErases.size();
  }

  static size_t GetDirtyEntryCount(const NetworkTablesTunableBackend& backend) {
    return backend.m_dirtyEntries.size();
  }

  static size_t GetAlwaysGetEntryCount(
      const NetworkTablesTunableBackend& backend) {
    return backend.m_alwaysGetEntries.size();
  }
};
}  // namespace wpi::backend

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest PublishesAndTunesScalarDataTypes",
    "[wpilibc][tunable]") {
  auto config = RobustConfig();
  wpi::tunables::TunableBool boolean{true, config};
  wpi::tunables::TunableInt32 intValue{1, config};
  wpi::tunables::TunableInt64 longValue{2, config};
  wpi::tunables::TunableFloat floatValue{3.25f, config};
  wpi::tunables::TunableDouble doubleValue{4.5, config};
  wpi::tunables::TunableString stringValue{"ready", config};

  wpi::tunables::Publish("boolean", boolean);
  wpi::tunables::Publish("int", intValue);
  wpi::tunables::Publish("long", longValue);
  wpi::tunables::Publish("float", floatValue);
  wpi::tunables::Publish("double", doubleValue);
  wpi::tunables::Publish("string", stringValue);

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
  wpi::tunables::TunableRegistry::Update();

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
  wpi::tunables::TunableRaw raw{std::vector<uint8_t>{1, 2}, config};
  wpi::tunables::TunableBoolVector booleans{std::vector<bool>{true, false},
                                            config};
  wpi::tunables::TunableInt32Vector ints{std::vector<int32_t>{3, 4}, config};
  wpi::tunables::TunableInt64Vector longs{std::vector<int64_t>{5, 6}, config};
  wpi::tunables::TunableFloatVector floats{std::vector<float>{7.25f, 8.5f},
                                           config};
  wpi::tunables::TunableDoubleVector doubles{std::vector<double>{9.25, 10.5},
                                             config};
  wpi::tunables::Tunable<std::vector<std::string>> strings{
      std::vector<std::string>{"a", "b"}, config};

  wpi::tunables::Publish("raw", raw);
  wpi::tunables::Publish("booleans", booleans);
  wpi::tunables::Publish("ints", ints);
  wpi::tunables::Publish("longs", longs);
  wpi::tunables::Publish("floats", floats);
  wpi::tunables::Publish("doubles", doubles);
  wpi::tunables::Publish("strings", strings);

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
  wpi::tunables::TunableRegistry::Update();

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
  wpi::tunables::TunableConfig config;
  wpi::tunables::TunableDouble value{1.0, config};
  wpi::tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo").Subscribe(0.0);
  CHECK(sub.Get() == 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo").Publish();
  pub.Set(2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(value.Get() == 2.0);
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest TunablesWithoutConfigAreMutable",
    "[wpilibc][tunable]") {
  wpi::tunables::TunableDouble value{1.0};
  wpi::tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo").Subscribe(0.0);
  CHECK(sub.Get() == 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo").Publish();
  pub.Set(2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(value.Get() == 2.0);
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest PublishesRobustDouble",
                 "[wpilibc][tunable]") {
  wpi::tunables::TunableConfig config;
  config.robust = true;
  wpi::tunables::TunableDouble value{1.0, config};
  wpi::tunables::Publish("foo", value);

  auto sub = inst.GetDoubleTopic("/Tunables/foo/value").Subscribe(0.0);
  CHECK(sub.Get() == 1.0);

  auto pub = inst.GetDoubleTopic("/Tunables/foo/tune").Publish();
  pub.Set(2.0);
  wpi::tunables::TunableRegistry::Update();

  CHECK(value.Get() == 2.0);
  CHECK(sub.Get() == 2.0);
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "SelectablePublishesOptionChangesAfterPublish",
                 "[wpilibc][tunable]") {
  wpi::tunables::Selectable<int> chooser;
  chooser.Add("one", 1);
  wpi::tunables::Publish("chooser", chooser);

  auto sub =
      inst.GetStringArrayTopic("/Tunables/chooser/options").Subscribe({});
  CHECK((std::vector<std::string>{"one"}) == sub.Get());

  chooser.Add("two", 2);
  wpi::tunables::TunableRegistry::Update();

  CHECK((std::vector<std::string>{"one", "two"}) == sub.Get());

  chooser.Add("one", 11);
  wpi::tunables::TunableRegistry::Update();

  CHECK((std::vector<std::string>{"two", "one"}) == sub.Get());

  chooser.Clear();
  wpi::tunables::TunableRegistry::Update();

  CHECK((std::vector<std::string>{}) == sub.Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "SelectableDashboardConnectsAfterPublish",
                 "[wpilibc][tunable]") {
  wpi::tunables::Selectable<int> chooser;
  ConfigureSelectable(chooser);
  wpi::tunables::Publish("auto", chooser);

  DashboardSelectable dashboard{inst, "/Tunables/auto"};

  CHECK(dashboard.Exists());
  CHECK("one" == dashboard.GetDefault());
  CHECK((std::vector<std::string>{"one", "two"}) == dashboard.GetOptions());
  CHECK("" == dashboard.GetSelected());
  CHECK("one" == dashboard.GetActive());
  CHECK(1 == chooser.GetSelected());

  dashboard.SetSelected("two");
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK("two" == dashboard.GetSelected());
  CHECK("two" == dashboard.GetActive());
  CHECK(2 == chooser.GetSelected());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "SelectableDashboardConnectsBeforePublish",
                 "[wpilibc][tunable]") {
  DashboardSelectable dashboard{inst, "/Tunables/auto"};
  wpi::tunables::Selectable<int> chooser;
  ConfigureSelectable(chooser);

  wpi::tunables::Publish("auto", chooser);

  CHECK(dashboard.Exists());
  CHECK("one" == dashboard.GetDefault());
  CHECK((std::vector<std::string>{"one", "two"}) == dashboard.GetOptions());
  CHECK("one" == dashboard.GetActive());
  CHECK(1 == chooser.GetSelected());

  dashboard.SetSelected("two");
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK("two" == dashboard.GetSelected());
  CHECK("two" == dashboard.GetActive());
  CHECK(2 == chooser.GetSelected());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "SelectableRetainsDashboardSelectionAfterRepublish",
                 "[wpilibc][tunable]") {
  DashboardSelectable dashboard{inst, "/Tunables/auto"};
  {
    wpi::tunables::Selectable<int> chooser;
    ConfigureSelectable(chooser);
    wpi::tunables::Publish("auto", chooser);

    dashboard.SetSelected("two");
    inst.Flush();
    wpi::tunables::TunableRegistry::Update();

    CHECK("two" == dashboard.GetSelected());
    CHECK("two" == dashboard.GetActive());
    CHECK(2 == chooser.GetSelected());

    RestartRobotBackend();
  }

  wpi::tunables::Selectable<int> chooser;
  ConfigureSelectable(chooser);
  wpi::tunables::Publish("auto", chooser);
  wpi::tunables::TunableRegistry::Update();

  CHECK("two" == dashboard.GetSelected());
  CHECK("two" == dashboard.GetActive());
  CHECK(2 == chooser.GetSelected());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest PublishesAndTunesStruct",
                 "[wpilibc][tunable]") {
  const wpi::math::Translation2d initial{1.25_m, 2.5_m};
  const wpi::math::Translation2d tuned{3.75_m, 4.5_m};
  wpi::tunables::TunableConfig config;
  config.robust = true;
  wpi::tunables::Tunable<wpi::math::Translation2d> value{config, initial};
  wpi::tunables::Publish("translation", value);

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
  wpi::tunables::TunableRegistry::Update();

  CHECK(tuned.X() == value.Get().X());
  CHECK(tuned.Y() == value.Get().Y());
  logged = sub.Get();
  CHECK(tuned.X() == logged.X());
  CHECK(tuned.Y() == logged.Y());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "IgnoresInvalidStructTunePayloads",
                 "[wpilibc][tunable]") {
  const wpi::math::Translation2d initial{1.25_m, 2.5_m};
  wpi::tunables::TunableConfig config;
  config.robust = true;
  int calls = 0;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) { ++calls; };
  wpi::tunables::Tunable<wpi::math::Translation2d> value{config, initial};
  wpi::tunables::Publish("translation", value);

  auto sub = inst.GetStructTopic<wpi::math::Translation2d>(
                     "/Tunables/translation/value")
                 .Subscribe({});
  auto logged = sub.Get();
  CHECK(initial.X() == logged.X());
  CHECK(initial.Y() == logged.Y());
  sub.ReadQueue();

  auto pub =
      inst.GetTopic("/Tunables/translation/tune")
          .GenericPublish(std::string{
              wpi::util::GetStructTypeString<wpi::math::Translation2d>()});

  pub.SetRaw(std::vector<uint8_t>{1, 2, 3});
  inst.Flush();
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());

  CHECK(initial.X() == value.Get().X());
  CHECK(initial.Y() == value.Get().Y());

  std::vector<uint8_t> oversized(
      wpi::util::GetStructSize<wpi::math::Translation2d>() + 1);
  pub.SetRaw(oversized);
  inst.Flush();
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());

  CHECK(initial.X() == value.Get().X());
  CHECK(initial.Y() == value.Get().Y());
  CHECK(0 == calls);
  CHECK(sub.ReadQueue().empty());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "IgnoresInvalidStructArrayTunePayloads",
                 "[wpilibc][tunable]") {
  const std::vector<wpi::math::Translation2d> initial{{1.25_m, 2.5_m},
                                                      {3.5_m, 4.75_m}};
  wpi::tunables::TunableConfig config;
  config.robust = true;
  int calls = 0;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) { ++calls; };
  wpi::tunables::Tunable<std::vector<wpi::math::Translation2d>> value{config,
                                                                      initial};
  wpi::tunables::Publish("translations", value);

  auto sub = inst.GetStructArrayTopic<wpi::math::Translation2d>(
                     "/Tunables/translations/value")
                 .Subscribe({});
  auto logged = sub.Get();
  REQUIRE(initial.size() == logged.size());
  CHECK(initial[0].X() == logged[0].X());
  CHECK(initial[0].Y() == logged[0].Y());
  CHECK(initial[1].X() == logged[1].X());
  CHECK(initial[1].Y() == logged[1].Y());
  sub.ReadQueue();

  std::string typeString{
      wpi::util::MakeStructArrayTypeString<wpi::math::Translation2d,
                                           std::dynamic_extent>()};
  CHECK(typeString ==
        inst.GetTopic("/Tunables/translations/value").GetTypeString());

  auto pub =
      inst.GetTopic("/Tunables/translations/tune").GenericPublish(typeString);

  std::vector<uint8_t> partial(
      wpi::util::GetStructSize<wpi::math::Translation2d>() + 1);
  pub.SetRaw(partial);
  inst.Flush();
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());

  REQUIRE(initial.size() == value.Get().size());
  CHECK(initial[0].X() == value.Get()[0].X());
  CHECK(initial[0].Y() == value.Get()[0].Y());
  CHECK(initial[1].X() == value.Get()[1].X());
  CHECK(initial[1].Y() == value.Get()[1].Y());
  CHECK(0 == calls);
  CHECK(sub.ReadQueue().empty());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest PublishesAndTunesProtobuf",
                 "[wpilibc][tunable]") {
  const wpi::math::Translation2d initial{5.25_m, 6.5_m};
  const wpi::math::Translation2d tuned{7.75_m, 8.5_m};
  wpi::tunables::TunableConfig config;
  config.robust = true;
  wpi::tunables::detail::TunableProtobuf<wpi::math::Translation2d> value{
      config, initial};
  wpi::tunables::Publish("translation", value);

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
  wpi::tunables::TunableRegistry::Update();

  CHECK(tuned.X() == value.Get().X());
  CHECK(tuned.Y() == value.Get().Y());
  logged = sub.Get();
  CHECK(tuned.X() == logged.X());
  CHECK(tuned.Y() == logged.Y());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "EmptyProtobufPayloadTunesToDefaultInstance",
                 "[wpilibc][tunable]") {
  const wpi::math::Translation2d initial{5.25_m, 6.5_m};
  const wpi::math::Translation2d zero;
  wpi::tunables::TunableConfig config;
  config.robust = true;
  wpi::tunables::detail::TunableProtobuf<wpi::math::Translation2d> value{
      config, initial};
  wpi::tunables::Publish("defaultTranslation", value);

  auto sub = inst.GetProtobufTopic<wpi::math::Translation2d>(
                     "/Tunables/defaultTranslation/value")
                 .Subscribe({});
  auto pub = inst.GetProtobufTopic<wpi::math::Translation2d>(
                     "/Tunables/defaultTranslation/tune")
                 .Publish();
  pub.Set(zero);
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK(zero.X() == value.Get().X());
  CHECK(zero.Y() == value.Get().Y());
  auto logged = sub.Get();
  CHECK(zero.X() == logged.X());
  CHECK(zero.Y() == logged.Y());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "RejectsMalformedProtobufTunePayloads",
                 "[wpilibc][tunable]") {
  const wpi::math::Translation2d initial{5.25_m, 6.5_m};
  const wpi::math::Translation2d partial{9.25_m, 10.5_m};
  int calls = 0;
  wpi::tunables::TunableConfig config;
  config.robust = true;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) { ++calls; };
  wpi::tunables::detail::TunableProtobuf<wpi::math::Translation2d> value{
      config, initial};
  wpi::tunables::Publish("malformedTranslation", value);

  auto sub = inst.GetProtobufTopic<wpi::math::Translation2d>(
                     "/Tunables/malformedTranslation/value")
                 .Subscribe({});
  auto logged = sub.Get();
  CHECK(initial.X() == logged.X());
  CHECK(initial.Y() == logged.Y());
  sub.ReadQueue();

  wpi::util::ProtobufMessage<wpi::math::Translation2d> message;
  std::vector<uint8_t> malformed;
  REQUIRE(message.Pack(malformed, partial));
  REQUIRE(malformed.size() > 10);
  malformed.resize(10);

  auto pub = inst.GetTopic("/Tunables/malformedTranslation/tune")
                 .GenericPublish(message.GetTypeString());
  std::vector<std::string> warnings;
  wpi::tunables::TunableRegistry::SetReportWarning(
      [&](std::string_view msg) { warnings.emplace_back(msg); });
  pub.SetRaw(malformed);
  inst.Flush();
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());

  CHECK(initial.X() == value.Get().X());
  CHECK(initial.Y() == value.Get().Y());
  CHECK(0 == calls);
  CHECK(sub.ReadQueue().empty());
  CHECK(HasWarning(warnings, "/Tunables/malformedTranslation",
                   "rejected protobuf tune payload"));
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest AppliesBackendConfigOptions",
                 "[wpilibc][tunable]") {
  int calls = 0;
  wpi::tunables::TunableConfig config;
  config.robust = true;
  config.typeString = "json";
  config.properties = wpi::util::json::object();
  config.properties["min"] = 0;
  config.properties["max"] = 10;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) { ++calls; };
  wpi::tunables::TunableString value{"1", config};
  wpi::tunables::Publish("configured", value);

  auto topic = inst.GetTopic("/Tunables/configured/value");
  CHECK("json" == topic.GetTypeString());
  CHECK(true == topic.GetProperty("robust"));
  CHECK(true == topic.GetProperty("mutable"));
  CHECK(0 == topic.GetProperty("min"));
  CHECK(10 == topic.GetProperty("max"));
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest OnTuneRunsForMutableRemoteUpdates",
    "[wpilibc][tunable]") {
  int calls = 0;
  wpi::tunables::TunableConfig config;
  config.robust = true;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) { ++calls; };
  wpi::tunables::TunableDouble value{1.0, config};
  wpi::tunables::Publish("mutable", value);

  auto pub = Tune("mutable", "double");
  pub.SetDouble(2.0);
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK(2.0 == value.Get());
  CHECK(1 == calls);
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "RemoteSetCallbackCanCanonicalizeRobustEcho",
                 "[wpilibc][tunable]") {
  wpi::tunables::TunableDouble* valuePtr = nullptr;
  wpi::tunables::TunableConfig config;
  config.robust = true;
  config.onRemoteSet = [&](wpi::tunables::detail::TunableBase&,
                           wpi::tunables::ComplexTunable*) {
    if (valuePtr->Get() > 5.0) {
      valuePtr->Set(5.0);
    }
  };
  wpi::tunables::TunableDouble value{1.0, config};
  valuePtr = &value;
  wpi::tunables::Publish("clamped", value);

  auto sub = inst.GetDoubleTopic("/Tunables/clamped/value").Subscribe(0.0);
  CHECK(1.0 == sub.Get());

  auto pub = Tune("clamped", "double");
  pub.SetDouble(10.0);
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK(5.0 == value.Get());
  CHECK(5.0 == sub.Get());
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest RejectsStructTunesThatThrowDuringUnpack",
    "[wpilibc][tunable]") {
  auto standaloneConfig = RobustConfig();
  int standaloneCalls = 0;
  standaloneConfig.onTune = [&](wpi::tunables::detail::TunableBase&,
                                wpi::tunables::ComplexTunable*) {
    ++standaloneCalls;
  };
  wpi::tunables::Tunable<ThrowingPackStruct> throwing{standaloneConfig,
                                                      ThrowingPackStruct{1}};
  wpi::tunables::Publish("throwing", throwing);

  auto memberConfig = RobustConfig();
  int memberCalls = 0;
  memberConfig.onTune = [&](wpi::tunables::detail::TunableBase&,
                            wpi::tunables::ComplexTunable*) { ++memberCalls; };
  StructMemberComplexTunable complex{memberConfig};
  wpi::tunables::Publish("complex", complex);

  auto throwingPub =
      inst.GetStructTopic<ThrowingPackStruct>("/Tunables/throwing/tune")
          .Publish();
  auto memberPub =
      inst.GetStructTopic<ThrowingPackStruct>("/Tunables/complex/member/tune")
          .Publish();
  std::vector<std::string> warnings;
  wpi::tunables::TunableRegistry::SetReportWarning(
      [&](std::string_view msg) { warnings.emplace_back(msg); });

  ThrowingPackStructState::throwDuringUnpack = true;
  throwingPub.Set({2});
  inst.Flush();
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());

  CHECK(1 == throwing.Get().value);
  CHECK(0 == standaloneCalls);
  CHECK(HasWarning(warnings, "/Tunables/throwing",
                   "rejected struct tune payload"));

  ThrowingPackStructState::throwDuringUnpack = true;
  memberPub.Set({3});
  inst.Flush();
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());

  CHECK(1 == complex.value.value);
  CHECK(0 == memberCalls);
  CHECK(HasWarning(warnings, "/Tunables/complex/member",
                   "rejected struct tune payload"));

  throwingPub.Set({4});
  memberPub.Set({5});
  inst.Flush();
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());

  CHECK(4 == throwing.Get().value);
  CHECK(5 == complex.value.value);
  CHECK(1 == standaloneCalls);
  CHECK(1 == memberCalls);
  ThrowingPackStructState::throwDuringUnpack = false;
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest OnTuneCanPublishAndRemoveTunables",
    "[wpilibc][tunable]") {
  bool callbackRan = false;
  wpi::tunables::TunableDouble published{3.0};
  wpi::tunables::TunableDouble removeMe{4.0};
  wpi::tunables::Publish("removeMe", removeMe);

  wpi::tunables::TunableConfig config;
  config.robust = true;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) {
    if (callbackRan) {
      return;
    }
    callbackRan = true;
    wpi::tunables::Remove("removeMe");
    wpi::tunables::Publish("publishedFromOnTune", published);
  };
  wpi::tunables::TunableDouble value{1.0, config};
  wpi::tunables::Publish("mutable", value);

  auto pub = Tune("mutable", "double");
  pub.SetDouble(2.0);
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK(callbackRan);
  CHECK(2.0 == value.Get());
  CHECK(3.0 == inst.GetDoubleTopic("/Tunables/publishedFromOnTune")
                   .Subscribe(0.0)
                   .Get());

  wpi::tunables::TunableDouble replacement{5.0};
  CHECK_NOTHROW(wpi::tunables::Publish("removeMe", replacement));
  CHECK(5.0 == inst.GetDoubleTopic("/Tunables/removeMe").Subscribe(0.0).Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "FailureDuringNetworkUpdateAppliesDeferredErases",
                 "[wpilibc][tunable]") {
  wpi::tunables::TunableDouble removeMe{4.0};
  wpi::tunables::Publish("removeMe", removeMe);
  wpi::tunables::Tunable<ThrowingPackStruct> throwingStruct{
      ThrowingPackStruct{1}};
  wpi::tunables::Publish("throwingStruct", throwingStruct);

  ThrowingPackStructState::removeDuringPack = true;
  ThrowingPackStructState::throwDuringPack = true;
  throwingStruct.Set({2});

  std::vector<std::string> warnings;
  wpi::tunables::TunableRegistry::SetReportWarning(
      [&](std::string_view msg) { warnings.emplace_back(msg); });

  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());
  ThrowingPackStructState::throwDuringPack = false;
  CHECK(HasWarning(warnings, "NetworkTables tunable backend update failed",
                   "broken pack"));

  CHECK(0 ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetUpdateDepth(
            *backend));
  CHECK(0u == wpi::backend::NetworkTablesTunableBackendTestAccess::
                  GetDeferredEraseCount(*backend));

  wpi::tunables::TunableDouble replacement{5.0};
  CHECK_NOTHROW(wpi::tunables::Publish("removeMe", replacement));
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "InitialNetworkUpdateFailureRollsBackEntry",
                 "[wpilibc][tunable]") {
  auto config = wpi::tunables::TunableConfig::AlwaysGet();
  wpi::tunables::Tunable<ThrowingPackStruct> throwingStruct{
      config, ThrowingPackStruct{1}};

  std::vector<std::string> warnings;
  wpi::tunables::TunableRegistry::SetReportWarning(
      [&](std::string_view msg) { warnings.emplace_back(msg); });

  ThrowingPackStructState::throwDuringPack = true;
  CHECK_NOTHROW(wpi::tunables::Publish("throwingStruct", throwingStruct));
  ThrowingPackStructState::throwDuringPack = false;
  CHECK(HasWarning(warnings, "/Tunables/throwingStruct",
                   "failed during initial publish"));

  CHECK(0u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetEntryCount(
            *backend));
  CHECK(0u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetUidPathCount(
            *backend));
  CHECK(0u == wpi::backend::NetworkTablesTunableBackendTestAccess::
                  GetAlwaysGetEntryCount(*backend));

  CHECK_NOTHROW(wpi::tunables::Publish("throwingStruct", throwingStruct));

  CHECK(1u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetEntryCount(
            *backend));
  CHECK(1u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetUidPathCount(
            *backend));
  CHECK(1u == wpi::backend::NetworkTablesTunableBackendTestAccess::
                  GetAlwaysGetEntryCount(*backend));
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "ComplexUpdateCanPublishAndRemoveTunables",
                 "[wpilibc][tunable]") {
  wpi::tunables::TunableDouble published{3.0};
  wpi::tunables::TunableDouble removeMe{4.0};
  wpi::tunables::Publish("removeMe", removeMe);

  MutatingComplexTunable complex{published};
  wpi::tunables::Publish("complex", complex);
  CountingComplexTunable after;
  wpi::tunables::Publish("z", after);

  CHECK(true ==
        inst.GetTopic("/Tunables/complex/.type").GetProperty("mutable"));

  wpi::tunables::TunableRegistry::Update();

  CHECK(1 == complex.GetUpdates());
  CHECK(1 == after.GetUpdates());
  CHECK(3.0 == inst.GetDoubleTopic("/Tunables/publishedFromComplex")
                   .Subscribe(0.0)
                   .Get());

  wpi::tunables::TunableDouble replacement{5.0};
  CHECK_NOTHROW(wpi::tunables::Publish("removeMe", replacement));
  CHECK(5.0 == inst.GetDoubleTopic("/Tunables/removeMe").Subscribe(0.0).Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "NonRobustTunablesDoNotTuneFromLocalPublishes",
                 "[wpilibc][tunable]") {
  int calls = 0;
  wpi::tunables::TunableConfig config;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) { ++calls; };
  wpi::tunables::TunableDouble value{1.0, config};
  wpi::tunables::Publish("localPublish", value);

  CHECK(true == inst.GetTopic("/Tunables/localPublish").GetProperty("mutable"));

  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK(0 == calls);

  value = 2.0;
  wpi::tunables::TunableRegistry::Update();
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  auto sub = inst.GetDoubleTopic("/Tunables/localPublish").Subscribe(0.0);
  CHECK(2.0 == value.Get());
  CHECK(2.0 == sub.Get());
  CHECK(0 == calls);
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "ProgrammaticChangesQueueDirtyEntries",
                 "[wpilibc][tunable]") {
  wpi::tunables::TunableDouble changed{1.0};
  wpi::tunables::TunableDouble unchanged{2.0};
  auto alwaysGetConfig = wpi::tunables::TunableConfig::AlwaysGet();
  wpi::tunables::TunableDouble alwaysGet{3.0, alwaysGetConfig};
  wpi::tunables::Publish("changed", changed);
  wpi::tunables::Publish("unchanged", unchanged);
  wpi::tunables::Publish("alwaysGet", alwaysGet);

  CHECK(0u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetDirtyEntryCount(
            *backend));
  CHECK(1u == wpi::backend::NetworkTablesTunableBackendTestAccess::
                  GetAlwaysGetEntryCount(*backend));

  changed.Set(4.0);
  changed.Set(5.0);
  alwaysGet.Set(6.0);

  CHECK(1u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetDirtyEntryCount(
            *backend));

  wpi::tunables::TunableRegistry::Update();

  CHECK(0u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetDirtyEntryCount(
            *backend));
  CHECK(5.0 == inst.GetDoubleTopic("/Tunables/changed").Subscribe(0.0).Get());
  CHECK(2.0 == inst.GetDoubleTopic("/Tunables/unchanged").Subscribe(0.0).Get());
  CHECK(6.0 == inst.GetDoubleTopic("/Tunables/alwaysGet").Subscribe(0.0).Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "FailureDuringDirtyNetworkUpdateClearsDirtyEntries",
                 "[wpilibc][tunable]") {
  wpi::tunables::Tunable<ThrowingPackStruct> throwingStruct{
      ThrowingPackStruct{1}};
  wpi::tunables::TunableDouble after{2.0};
  wpi::tunables::Publish("throwingStruct", throwingStruct);
  wpi::tunables::Publish("after", after);

  throwingStruct.Set({3});
  after.Set(4.0);
  CHECK(2u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetDirtyEntryCount(
            *backend));

  std::vector<std::string> warnings;
  wpi::tunables::TunableRegistry::SetReportWarning(
      [&](std::string_view msg) { warnings.emplace_back(msg); });

  ThrowingPackStructState::throwDuringPack = true;
  CHECK_NOTHROW(wpi::tunables::TunableRegistry::Update());
  ThrowingPackStructState::throwDuringPack = false;
  CHECK(HasWarning(warnings, "NetworkTables tunable backend update failed",
                   "broken pack"));

  CHECK(0 ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetUpdateDepth(
            *backend));
  CHECK(0u ==
        wpi::backend::NetworkTablesTunableBackendTestAccess::GetDirtyEntryCount(
            *backend));
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "ProgrammaticChangesPublishToAliasesAcrossBackends",
                 "[wpilibc][tunable]") {
  auto childBackend =
      std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
          inst, "/ChildTunables");
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);
  wpi::tunables::TunableConfig config = RobustConfig();
  wpi::tunables::TunableDouble value{1.0, config};
  wpi::tunables::Publish("sharedA", value);
  wpi::tunables::Publish("sharedB", value);
  wpi::tunables::Publish("child/shared", value);

  CHECK(1.0 ==
        inst.GetDoubleTopic("/Tunables/sharedA/value").Subscribe(0.0).Get());
  CHECK(1.0 ==
        inst.GetDoubleTopic("/Tunables/sharedB/value").Subscribe(0.0).Get());
  CHECK(1.0 == inst.GetDoubleTopic("/ChildTunables/child/shared/value")
                   .Subscribe(0.0)
                   .Get());
  CHECK(true == inst.GetTopic("/ChildTunables/child/shared/value")
                    .GetProperty("mutable"));

  value = 2.0;
  wpi::tunables::TunableRegistry::Update();

  CHECK(2.0 ==
        inst.GetDoubleTopic("/Tunables/sharedA/value").Subscribe(0.0).Get());
  CHECK(2.0 ==
        inst.GetDoubleTopic("/Tunables/sharedB/value").Subscribe(0.0).Get());
  CHECK(2.0 == inst.GetDoubleTopic("/ChildTunables/child/shared/value")
                   .Subscribe(0.0)
                   .Get());
}

TEST_CASE_METHOD(NetworkTablesTunableBackendTest,
                 "NetworkTablesTunableBackendTest "
                 "BackendMigrationUsesPathBoundaries",
                 "[wpilibc][tunable]") {
  wpi::tunables::TunableConfig config = RobustConfig();
  wpi::tunables::TunableDouble child{1.0, config};
  wpi::tunables::TunableDouble children{2.0, config};
  wpi::tunables::Publish("child/value", child);
  wpi::tunables::Publish("children/value", children);

  auto childBackend =
      std::make_shared<wpi::backend::NetworkTablesTunableBackend>(
          inst, "/ChildTunables");
  wpi::tunables::TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK(1.0 == inst.GetDoubleTopic("/ChildTunables/child/value/value")
                   .Subscribe(0.0)
                   .Get());
  CHECK_FALSE(inst.GetTopic("/ChildTunables/children/value/value").Exists());
  CHECK(2.0 == Value("children/value").GetDouble(0.0));
}

TEST_CASE_METHOD(
    NetworkTablesTunableBackendTest,
    "NetworkTablesTunableBackendTest ImmutableTunablesIgnoreRemoteUpdates",
    "[wpilibc][tunable]") {
  int calls = 0;
  wpi::tunables::TunableConfig config;
  config.robust = true;
  config.isMutable = false;
  config.onTune = [&](wpi::tunables::detail::TunableBase&,
                      wpi::tunables::ComplexTunable*) { ++calls; };
  wpi::tunables::TunableDouble value{1.0, config};
  wpi::tunables::Publish("immutable", value);

  CHECK(false ==
        inst.GetTopic("/Tunables/immutable/value").GetProperty("mutable"));

  auto pub = Tune("immutable", "double");
  pub.SetDouble(2.0);
  inst.Flush();
  wpi::tunables::TunableRegistry::Update();

  CHECK(1.0 == value.Get());
  CHECK(0 == calls);
}
