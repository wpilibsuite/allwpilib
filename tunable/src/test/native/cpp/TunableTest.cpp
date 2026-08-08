// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Tunable.hpp"

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

using namespace wpi;

TEST_CASE("TunableRegistryTest ReportWarning", "[tunable]") {
  std::string lastWarning;
  TunableRegistry::SetReportWarning(
      [&lastWarning](std::string_view msg) { lastWarning = msg; });
  TunableRegistry::ReportWarning("test warning");
  CHECK(lastWarning == "test warning");
  TunableRegistry::SetReportWarning(nullptr);
}

static_assert(std::derived_from<TunableBool, detail::TunableValueBase<bool>>);
static_assert(
    std::derived_from<TunableInt32, detail::TunableValueBase<int32_t>>);
static_assert(
    std::derived_from<TunableInt64, detail::TunableValueBase<int64_t>>);
static_assert(std::derived_from<TunableFloat, detail::TunableValueBase<float>>);
static_assert(
    std::derived_from<TunableDouble, detail::TunableValueBase<double>>);
static_assert(std::derived_from<
              TunableRaw, detail::TunableValueBase<std::vector<uint8_t>>>);
static_assert(std::derived_from<TunableBoolVector,
                                detail::TunableValueBase<std::vector<bool>>>);
static_assert(
    std::derived_from<TunableInt32Vector,
                      detail::TunableValueBase<std::vector<int32_t>>>);
static_assert(
    std::derived_from<TunableInt64Vector,
                      detail::TunableValueBase<std::vector<int64_t>>>);
static_assert(std::derived_from<TunableFloatVector,
                                detail::TunableValueBase<std::vector<float>>>);
static_assert(std::derived_from<TunableDoubleVector,
                                detail::TunableValueBase<std::vector<double>>>);
static_assert(std::same_as<decltype(TunableRegistry::GetUpdateMutex()),
                           wpi::util::recursive_mutex&>);

namespace {
struct TunableTest {
  TunableTest() { TunableRegistry::RegisterBackend("", backend); }

  ~TunableTest() { TunableRegistry::Reset(); }

  std::shared_ptr<MockTunableBackend> backend =
      std::make_shared<MockTunableBackend>();
};

struct TestStruct {
  int16_t a;
  int16_t b;
};

struct CustomType {
  int32_t val;
};

template <typename T>
struct CustomType2 {
  T val;
};

template <typename T>
class CustomTunableType2 {
 public:
  CustomTunableType2() = default;
  explicit CustomTunableType2(const CustomType2<T>& val) : m_tunable{val.val} {}

  CustomType2<T> Get() const { return CustomType2<T>{m_tunable.Get()}; }
  void Set(CustomType2<T> value) { m_tunable = value.val; }

  Tunable<T>& GetInnerTunable() { return m_tunable; }

 private:
  Tunable<T> m_tunable;
};

template <typename T>
constexpr CustomTunableType2<T> MakeTunable(const CustomType2<T>& val) {
  return {val};
}
}  // namespace

template <typename T>
class MyTest : public decltype(MakeTunable(std::declval<T>())){};

MyTest<CustomType2<int32_t>> test;

static_assert(std::same_as<decltype(MakeTunable(CustomType2<int32_t>{})),
                           CustomTunableType2<int32_t>>);

static_assert(std::derived_from<MyTest<CustomType2<int32_t>>,
                                CustomTunableType2<int32_t>>);

template <>
class wpi::CustomTunable<CustomType> {
 public:
  CustomTunable() = default;
  explicit CustomTunable(const CustomType& val) : m_tunable{val.val} {}

  CustomType Get() const { return CustomType{m_tunable.Get()}; }
  void Set(CustomType value) { m_tunable = value.val; }

  Tunable<int32_t>& GetInnerTunable() { return m_tunable; }

 private:
  Tunable<int32_t> m_tunable;
};

static_assert(wpi::detail::CustomTunableType<CustomType>);

class InspectableDoubleTunable : public TunableDouble {
 public:
  using TunableDouble::TunableDouble;

  uint32_t GetUid() const { return GetTunableUid(); }
};

struct AssignableComplex : public ComplexTunable {
  explicit AssignableComplex(double value) : gain{value} {}

  uint32_t GetUid() const { return GetTunableUid(); }
  uint32_t GetGainUid() const { return gain.GetUid(); }
  double GetGain() const { return gain.Get(); }

  void PublishTunable(TunableTable& table) override {
    table.Publish("gain", gain);
  }

  InspectableDoubleTunable gain;
};

template <>
struct wpi::util::Struct<TestStruct> {
  static constexpr std::string_view GetTypeName() { return "TestStruct"; }
  static constexpr size_t GetSize() { return 4; }
  static constexpr std::string_view GetSchema() { return "int16 a;int16 b"; }
  static TestStruct Unpack(std::span<const uint8_t> data) {
    return {wpi::util::UnpackStruct<int16_t, 0>(data),
            wpi::util::UnpackStruct<int16_t, 2>(data)};
  }
  static void Pack(std::span<uint8_t> data, TestStruct value) {
    wpi::util::PackStruct<0>(data, value.a);
    wpi::util::PackStruct<2>(data, value.b);
  }
};

struct MemberComplex : public ComplexTunable {
  int32_t gain = 1;
  TestStruct point{2, 3};
  int updateCount = 0;

  void PublishTunable(TunableTable& table) override {
    table.Publish("gain", this, &MemberComplex::gain);
    table.Publish("point", this, &MemberComplex::point);
  }

  void UpdateTunable() const override {
    ++const_cast<MemberComplex*>(this)->updateCount;
  }
};

struct DirectStructComplex : public ComplexTunable {
  TestStruct point{2, 3};

  void PublishTunable(TunableTable& table) override {
    table.Publish("point", this, &DirectStructComplex::point);
  }
};

struct WrappedStructComplex : public ComplexTunable {
  Tunable<TestStruct> point{TestStruct{2, 3}};

  void PublishTunable(TunableTable& table) override {
    table.Publish("point", point);
  }
};

TEST_CASE_METHOD(TunableTest, "TunableTest IntTunable", "[tunable]") {
  Tunable<int32_t> tunable;

  tunable.Set(42);
  int32_t val = tunable.Get();
  CHECK(val == 42);

  tunable = 63;
  val = tunable;
  CHECK(val == 63);

  Tunables::Publish("test", tunable);
  backend->SetInt32("/test", 84);
  TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val == 84);
}

TEST_CASE_METHOD(TunableTest, "TunableTest PrimitiveAndVectorTunables",
                 "[tunable]") {
  TunableBool boolean{true};
  TunableInt64 integer64{1};
  TunableFloat floatValue{2.0f};
  TunableDouble doubleValue{3.0};
  TunableString stringValue{"start"};
  TunableInt32Vector vectorValue{std::vector<int32_t>{1, 2}};

  Tunables::Publish("boolean", boolean);
  Tunables::Publish("integer64", integer64);
  Tunables::Publish("float", floatValue);
  Tunables::Publish("double", doubleValue);
  Tunables::Publish("string", stringValue);
  Tunables::Publish("vector", vectorValue);

  backend->SetBool("/boolean", false);
  backend->SetInt64("/integer64", 10);
  backend->SetFloat("/float", 20.0f);
  backend->SetDouble("/double", 30.0);
  backend->SetString("/string", "remote");
  std::vector<int32_t> remoteVector{3, 4};
  backend->SetInt32Vector("/vector", remoteVector);
  remoteVector[0] = 99;
  TunableRegistry::Update();

  CHECK_FALSE(boolean.Get());
  CHECK(integer64.Get() == 10);
  CHECK(floatValue.Get() == 20.0f);
  CHECK(doubleValue.Get() == 30.0);
  CHECK(stringValue.Get() == "remote");
  CHECK(vectorValue.Get() == (std::vector<int32_t>{3, 4}));
}

TEST_CASE_METHOD(TunableTest, "TunableTest MutateMarksTunablesChanged",
                 "[tunable]") {
  class InspectableInt : public TunableInt32 {
   public:
    using TunableInt32::TunableInt32;

    uint32_t GetUid() const { return GetTunableUid(); }
  };
  class InspectableVector : public TunableInt32Vector {
   public:
    using TunableInt32Vector::TunableInt32Vector;

    uint32_t GetUid() const { return GetTunableUid(); }
  };

  TunableConfig config;
  InspectableInt integer{1, config};
  InspectableVector vector{std::vector<int32_t>{1, 2}, config};

  auto integerInfo = TunableRegistry::GetTunable(integer.GetUid());
  auto vectorInfo = TunableRegistry::GetTunable(vector.GetUid());
  REQUIRE(integerInfo);
  REQUIRE(vectorInfo);
  CHECK_FALSE(integerInfo.IsChanged());
  CHECK_FALSE(vectorInfo.IsChanged());

  integer.Mutate() = 2;
  vector.Mutate().push_back(3);

  integerInfo = TunableRegistry::GetTunable(integer.GetUid());
  vectorInfo = TunableRegistry::GetTunable(vector.GetUid());
  CHECK(integer.Get() == 2);
  CHECK(vector.Get() == (std::vector<int32_t>{1, 2, 3}));
  CHECK(integerInfo.IsChanged());
  CHECK(vectorInfo.IsChanged());
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest AssignmentPreservesPublishedDestinationRegistration",
    "[tunable]") {
  uint32_t copySourceUid;
  uint32_t copyDestinationUid;
  {
    InspectableDoubleTunable source{2.0};
    InspectableDoubleTunable destination{1.0};
    Tunables::Publish("copySource", source);
    Tunables::Publish("copyDestination", destination);
    copySourceUid = source.GetUid();
    copyDestinationUid = destination.GetUid();

    destination = source;

    CHECK(source.GetUid() == copySourceUid);
    CHECK(destination.GetUid() == copyDestinationUid);
    CHECK(destination.Get() == 2.0);
    REQUIRE(TunableRegistry::GetTunable(copyDestinationUid));
    CHECK(TunableRegistry::GetTunable(copyDestinationUid).IsChanged());

    backend->SetDouble("/copySource", 3.0);
    backend->SetDouble("/copyDestination", 4.0);
    TunableRegistry::Update();

    CHECK(source.Get() == 3.0);
    CHECK(destination.Get() == 4.0);
  }
  CHECK_FALSE(TunableRegistry::GetTunable(copySourceUid));
  CHECK_FALSE(TunableRegistry::GetTunable(copyDestinationUid));

  uint32_t moveSourceUid;
  uint32_t moveDestinationUid;
  {
    InspectableDoubleTunable source{6.0};
    InspectableDoubleTunable destination{5.0};
    Tunables::Publish("moveSource", source);
    Tunables::Publish("moveDestination", destination);
    moveSourceUid = source.GetUid();
    moveDestinationUid = destination.GetUid();

    destination = std::move(source);

    CHECK(source.GetUid() == moveSourceUid);
    CHECK(destination.GetUid() == moveDestinationUid);
    CHECK(destination.Get() == 6.0);
    REQUIRE(TunableRegistry::GetTunable(moveDestinationUid));
    CHECK(TunableRegistry::GetTunable(moveDestinationUid).IsChanged());

    backend->SetDouble("/moveSource", 7.0);
    backend->SetDouble("/moveDestination", 8.0);
    TunableRegistry::Update();

    CHECK(source.Get() == 7.0);
    CHECK(destination.Get() == 8.0);
  }
  CHECK_FALSE(TunableRegistry::GetTunable(moveSourceUid));
  CHECK_FALSE(TunableRegistry::GetTunable(moveDestinationUid));
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest ComplexAssignmentPreservesPublishedDestinationRegistration",
    "[tunable]") {
  uint32_t copyComplexUid;
  uint32_t copyGainUid;
  {
    AssignableComplex source{2.0};
    AssignableComplex destination{1.0};
    Tunables::Publish("copyComplex", destination);
    copyComplexUid = destination.GetUid();
    copyGainUid = destination.GetGainUid();

    destination = source;

    CHECK(destination.GetUid() == copyComplexUid);
    CHECK(destination.GetGainUid() == copyGainUid);
    CHECK(destination.GetGain() == 2.0);
    REQUIRE(TunableRegistry::GetTunable(copyComplexUid));
    REQUIRE(TunableRegistry::GetTunable(copyGainUid));
    CHECK(TunableRegistry::GetTunable(copyComplexUid).IsChanged());
    CHECK(TunableRegistry::GetTunable(copyGainUid).IsChanged());

    backend->SetDouble("/copyComplex/gain", 3.0);
    TunableRegistry::Update();

    CHECK(destination.GetGain() == 3.0);
  }
  CHECK_FALSE(TunableRegistry::GetTunable(copyComplexUid));
  CHECK_FALSE(TunableRegistry::GetTunable(copyGainUid));

  uint32_t moveComplexUid;
  uint32_t moveGainUid;
  {
    AssignableComplex source{5.0};
    AssignableComplex destination{4.0};
    Tunables::Publish("moveComplex", destination);
    moveComplexUid = destination.GetUid();
    moveGainUid = destination.GetGainUid();

    destination = std::move(source);

    CHECK(destination.GetUid() == moveComplexUid);
    CHECK(destination.GetGainUid() == moveGainUid);
    CHECK(destination.GetGain() == 5.0);
    REQUIRE(TunableRegistry::GetTunable(moveComplexUid));
    REQUIRE(TunableRegistry::GetTunable(moveGainUid));
    CHECK(TunableRegistry::GetTunable(moveComplexUid).IsChanged());
    CHECK(TunableRegistry::GetTunable(moveGainUid).IsChanged());

    backend->SetDouble("/moveComplex/gain", 6.0);
    TunableRegistry::Update();

    CHECK(destination.GetGain() == 6.0);
  }
  CHECK_FALSE(TunableRegistry::GetTunable(moveComplexUid));
  CHECK_FALSE(TunableRegistry::GetTunable(moveGainUid));
}

TEST_CASE_METHOD(TunableTest, "TunableTest ConfigImmutableAndOnTune",
                 "[tunable]") {
  int calls = 0;
  TunableConfig mutableConfig{
      .onTune = [&](detail::TunableBase&, ComplexTunable*) { ++calls; }};
  TunableConfig immutableConfig{.isMutable = false,
                                .onTune = mutableConfig.onTune};
  TunableInt32 mutableTunable{0, mutableConfig};
  TunableInt32 immutableTunable{5, immutableConfig};

  Tunables::Publish("mutable", mutableTunable);
  Tunables::Publish("immutable", immutableTunable);

  backend->SetInt32("/mutable", 1);
  backend->SetInt32("/immutable", 42);
  CHECK(calls == 0);
  TunableRegistry::Update();

  CHECK(mutableTunable.Get() == 1);
  CHECK(immutableTunable.Get() == 5);
  CHECK(calls == 1);
}

TEST_CASE_METHOD(TunableTest, "TunableTest TunableConfigOptions", "[tunable]") {
  int calls = 0;
  TunableConfig config{
      .properties = wpi::util::json::object("min", 0),
      .robust = true,
      .typeString = "UnitTestWidget",
      .isMutable = false,
      .onTune = [&](detail::TunableBase&, ComplexTunable*) { ++calls; },
      .alwaysGet = true};
  class InspectableInt : public TunableInt32 {
   public:
    using TunableInt32::TunableInt32;

    uint32_t GetUid() const { return GetTunableUid(); }
  };
  InspectableInt tunable{1, config};
  auto info = TunableRegistry::GetTunable(tunable.GetUid());

  REQUIRE(static_cast<bool>(info));
  REQUIRE(info.config != nullptr);
  CHECK(info.config->robust);
  CHECK(info.config->properties.at("min") == 0);
  REQUIRE(info.config->typeString.has_value());
  CHECK(info.config->typeString.value() == "UnitTestWidget");
  CHECK_FALSE(info.config->isMutable);
  CHECK(info.config->alwaysGet);

  Tunables::Publish("configured", tunable);
  backend->SetInt32("/configured", 2);
  TunableRegistry::Update();

  CHECK(tunable.Get() == 1);
  CHECK(calls == 0);

  info.config->onTune(tunable, nullptr);
  CHECK(calls == 1);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest OnTuneReceivesMemberTunableAndParent",
                 "[tunable]") {
  int calls = 0;
  bool receivedMember = false;
  bool receivedParent = false;
  MemberComplex complex;
  TunableConfig config{.onTune = [&](detail::TunableBase& tunable,
                                     ComplexTunable* parent) {
    ++calls;
    receivedMember = &tunable != static_cast<detail::TunableBase*>(&complex);
    receivedParent = parent == &complex;
  }};

  Tunables::Publish("complexOnTune/gain", &complex, &MemberComplex::gain,
                    config);
  backend->SetInt32("/complexOnTune/gain", 7);
  TunableRegistry::Update();

  CHECK(complex.gain == 7);
  CHECK(calls == 1);
  CHECK(receivedMember);
  CHECK(receivedParent);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest DestroyingComplexUnregistersMemberTunables",
                 "[tunable]") {
  std::optional<uint32_t> parentUid;
  std::optional<uint32_t> gainUid;
  std::optional<uint32_t> pointUid;
  {
    MemberComplex complex;
    Tunables::Publish("destroyedComplex", complex);
    parentUid = backend->GetUid("/destroyedComplex");
    gainUid = backend->GetUid("/destroyedComplex/gain");
    pointUid = backend->GetUid("/destroyedComplex/point");

    REQUIRE(parentUid);
    REQUIRE(gainUid);
    REQUIRE(pointUid);

    backend->SetInt32("/destroyedComplex/gain", 7);
  }

  CHECK_FALSE(backend->GetUid("/destroyedComplex"));
  CHECK_FALSE(backend->GetUid("/destroyedComplex/gain"));
  CHECK_FALSE(backend->GetUid("/destroyedComplex/point"));
  CHECK_FALSE(TunableRegistry::GetTunable(*parentUid));
  CHECK_FALSE(TunableRegistry::GetTunable(*gainUid));
  CHECK_FALSE(TunableRegistry::GetTunable(*pointUid));
  CHECK_NOTHROW(TunableRegistry::Update());
}

TEST_CASE_METHOD(TunableTest, "TunableTest TunablesGetTableFacade",
                 "[tunable]") {
  Tunable<double> tunable;
  auto table = Tunables::GetTable("arm");

  table.Publish("speed", tunable);
  backend->SetDouble("/arm/speed", 2.0);
  TunableRegistry::Update();

  CHECK(tunable.Get() == 2.0);
}

TEST_CASE_METHOD(TunableTest, "TunableTest TablePathsRouteAndRemove",
                 "[tunable]") {
  auto childBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK(Tunables::GetTable().GetPath() == "/");
  CHECK(Tunables::GetTable("drive").GetPath() == "/drive/");
  CHECK(Tunables::GetTable("drive").GetTable("left").GetPath() ==
        "/drive/left/");

  TunableDouble root{1.0};
  TunableDouble child{2.0};
  Tunables::Publish("root", root);
  Tunables::Publish("child/value", child);

  backend->SetDouble("/root", 3.0);
  childBackend->SetDouble("/child/value", 4.0);
  TunableRegistry::Update();
  CHECK(root.Get() == 3.0);
  CHECK(child.Get() == 4.0);
  CHECK_THROWS_AS(backend->SetDouble("/child/value", 5.0), std::runtime_error);

  Tunables::Remove("child/value");
  CHECK_THROWS_AS(childBackend->SetDouble("/child/value", 6.0),
                  std::runtime_error);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest RegisterBackendMigratesExistingMatchingTunables",
                 "[tunable]") {
  TunableDouble root{1.0};
  TunableDouble child{2.0};
  Tunables::Publish("root", root);
  Tunables::Publish("child/value", child);

  backend->SetDouble("/root", 3.0);
  backend->SetDouble("/child/value", 4.0);
  TunableRegistry::Update();
  CHECK(root.Get() == 3.0);
  CHECK(child.Get() == 4.0);

  auto childBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK_THROWS_AS(backend->SetDouble("/child/value", 5.0), std::runtime_error);
  childBackend->SetDouble("/child/value", 6.0);
  backend->SetDouble("/root", 7.0);
  TunableRegistry::Update();

  CHECK(root.Get() == 7.0);
  CHECK(child.Get() == 6.0);
}

TEST_CASE_METHOD(
    TunableTest,
    "TunableTest RegisterBackendReplacementMigratesExistingTunables",
    "[tunable]") {
  TunableDouble tunable{1.0};
  Tunables::Publish("value", tunable);

  auto replacementBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("", replacementBackend);

  CHECK_THROWS_AS(backend->SetDouble("/value", 2.0), std::runtime_error);
  replacementBackend->SetDouble("/value", 3.0);
  TunableRegistry::Update();

  CHECK(tunable.Get() == 3.0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest RegisterBackendMigratesComplexTunable",
                 "[tunable]") {
  MemberComplex complex;
  Tunables::Publish("child/complex", complex);

  auto childBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("/child", childBackend);

  CHECK_THROWS_AS(backend->SetInt32("/child/complex/gain", 2),
                  std::runtime_error);
  childBackend->SetInt32("/child/complex/gain", 4);
  childBackend->SetStruct<TestStruct>("/child/complex/point", {5, 6});
  TunableRegistry::Update();

  CHECK(complex.gain == 4);
  CHECK(complex.point.a == 5);
  CHECK(complex.point.b == 6);
  CHECK(complex.updateCount == 1);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest MockBackendRemovePrefixReturnsMatchingTunables",
                 "[tunable]") {
  TunableDouble root{1.0};
  TunableDouble child{2.0};
  Tunables::Publish("root", root);
  Tunables::Publish("child/value", child);

  auto removed = backend->RemovePrefix("/child");

  REQUIRE(removed.size() == 1u);
  CHECK(removed[0].path == "/child/value");
  backend->SetDouble("/root", 3.0);
  CHECK_THROWS_AS(backend->SetDouble("/child/value", 4.0), std::runtime_error);
  TunableRegistry::Update();
  CHECK(root.Get() == 3.0);
  CHECK(child.Get() == 2.0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexTunablePublishesMembersAndUpdates",
                 "[tunable]") {
  MemberComplex complex;
  Tunables::Publish("complex", complex);

  backend->SetInt32("/complex/gain", 10);
  backend->SetStruct<TestStruct>("/complex/point", {11, 12});
  TunableRegistry::Update();

  CHECK(complex.gain == 10);
  CHECK(complex.point.a == 11);
  CHECK(complex.point.b == 12);
  CHECK(complex.updateCount == 1);
}

TEST_CASE_METHOD(TunableTest, "TunableTest RemoveComplexTunableRemovesMembers",
                 "[tunable]") {
  MemberComplex complex;
  Tunables::Publish("complex", complex);

  Tunables::Remove("complex");

  CHECK_THROWS_AS(backend->SetInt32("/complex/gain", 10), std::runtime_error);
  CHECK_THROWS_AS(backend->SetStruct<TestStruct>("/complex/point", {11, 12}),
                  std::runtime_error);
  TunableRegistry::Update();

  CHECK(complex.gain == 1);
  CHECK(complex.point.a == 2);
  CHECK(complex.point.b == 3);
  CHECK(complex.updateCount == 0);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexTunableDirectStructSerializableMember",
                 "[tunable]") {
  DirectStructComplex complex;
  Tunables::Publish("directStruct", complex);

  backend->SetStruct<TestStruct>("/directStruct/point", {4, 5});
  TunableRegistry::Update();

  CHECK(complex.point.a == 4);
  CHECK(complex.point.b == 5);
}

TEST_CASE_METHOD(TunableTest,
                 "TunableTest ComplexTunableWrappedStructSerializableMember",
                 "[tunable]") {
  WrappedStructComplex complex;
  Tunables::Publish("wrappedStruct", complex);

  backend->SetStruct<TestStruct>("/wrappedStruct/point", {6, 7});
  TunableRegistry::Update();

  CHECK(complex.point.Get().a == 6);
  CHECK(complex.point.Get().b == 7);
}

TEST_CASE_METHOD(TunableTest, "TunableTest CustomTunable", "[tunable]") {
  Tunable<CustomType> tunable;

  tunable.Set(CustomType{42});
  auto val = tunable.Get();
  CHECK(val.val == 42);

  tunable = CustomType{63};
  val = tunable;
  CHECK(val.val == 63);

  Tunables::Publish("testCustom", tunable);
  backend->SetInt32("/testCustom", 84);
  TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.val == 84);
}

TEST_CASE_METHOD(TunableTest, "TunableTest StructTunable", "[tunable]") {
  Tunable<TestStruct> tunable;

  tunable.Set({1, 2});
  auto val = tunable.Get();
  CHECK(val.a == 1);
  CHECK(val.b == 2);

  tunable = {2, 3};
  val = tunable;
  CHECK(val.a == 2);
  CHECK(val.b == 3);

  Tunables::Publish("testStruct", tunable);
  backend->SetStruct<TestStruct>("/testStruct", {3, 4});
  TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.a == 3);
  CHECK(val.b == 4);
}

TEST_CASE_METHOD(TunableTest, "TunableTest StructVectorTunable", "[tunable]") {
  Tunable<std::vector<TestStruct>> tunable;

  tunable.Set({{1, 2}, {3, 4}});
  auto val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 1);
  CHECK(val[0].b == 2);
  CHECK(val[1].a == 3);
  CHECK(val[1].b == 4);

  tunable = {{2, 3}, {4, 5}, {6, 7}};
  val = tunable;
  CHECK(val.size() == 3u);
  CHECK(val[0].a == 2);
  CHECK(val[0].b == 3);
  CHECK(val[1].a == 4);
  CHECK(val[1].b == 5);
  CHECK(val[2].a == 6);
  CHECK(val[2].b == 7);

  Tunables::Publish("testStructVector", tunable);
  backend->SetStructVector<TestStruct>("/testStructVector", {{{5, 6}, {7, 8}}});
  TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 5);
  CHECK(val[0].b == 6);
  CHECK(val[1].a == 7);
  CHECK(val[1].b == 8);
}

TEST_CASE_METHOD(TunableTest, "TunableTest StructArrayTunable", "[tunable]") {
  Tunable<std::array<TestStruct, 2>> tunable;

  tunable.Set({{{1, 2}, {3, 4}}});
  auto val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 1);
  CHECK(val[0].b == 2);
  CHECK(val[1].a == 3);
  CHECK(val[1].b == 4);

  tunable = {{{2, 3}, {4, 5}}};
  val = tunable;
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 2);
  CHECK(val[0].b == 3);
  CHECK(val[1].a == 4);
  CHECK(val[1].b == 5);

  Tunables::Publish("testStructArray", tunable);
  backend->SetStructVector<TestStruct>("/testStructArray", {{{5, 6}, {7, 8}}});
  TunableRegistry::Update();
  val = tunable.Get();
  CHECK(val.size() == 2u);
  CHECK(val[0].a == 5);
  CHECK(val[0].b == 6);
  CHECK(val[1].a == 7);
  CHECK(val[1].b == 8);
}
