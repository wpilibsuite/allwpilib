// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Tunable.hpp"

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "wpi/tunable/ComplexTunable.hpp"
#include "wpi/tunable/MockTunableBackend.hpp"
#include "wpi/tunable/TunableConfig.hpp"
#include "wpi/tunable/TunableRegistry.hpp"
#include "wpi/tunable/Tunables.hpp"

using namespace wpi;

TEST(TunableRegistryTest, ReportWarning) {
  std::string lastWarning;
  TunableRegistry::SetReportWarning([&lastWarning](std::string_view msg) {
    lastWarning = msg;
  });
  TunableRegistry::ReportWarning("test warning");
  EXPECT_EQ(lastWarning, "test warning");
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

namespace {
class TunableTest : public ::testing::Test {
 protected:
  void SetUp() override { TunableRegistry::RegisterBackend("", backend); }

  void TearDown() override { TunableRegistry::Reset(); }

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
class MyTest : public decltype(MakeTunable(std::declval<T>())) {

};

MyTest<CustomType2<int32_t>> test;

static_assert(std::same_as<decltype(MakeTunable(CustomType2<int32_t>{})), CustomTunableType2<int32_t>>);

static_assert(std::derived_from<MyTest<CustomType2<int32_t>>, CustomTunableType2<int32_t>>);

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

TEST_F(TunableTest, IntTunable) {
  Tunable<int32_t> tunable;

  tunable.Set(42);
  int32_t val = tunable.Get();
  EXPECT_EQ(val, 42);

  tunable = 63;
  val = tunable;
  EXPECT_EQ(val, 63);

  Tunables::Publish("test", tunable);
  backend->SetInt32("/test", 84);
  TunableRegistry::Update();
  val = tunable.Get();
  EXPECT_EQ(val, 84);
}

TEST_F(TunableTest, PrimitiveAndVectorTunables) {
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

  EXPECT_FALSE(boolean.Get());
  EXPECT_EQ(integer64.Get(), 10);
  EXPECT_EQ(floatValue.Get(), 20.0f);
  EXPECT_EQ(doubleValue.Get(), 30.0);
  EXPECT_EQ(stringValue.Get(), "remote");
  EXPECT_EQ(vectorValue.Get(), (std::vector<int32_t>{3, 4}));
}

TEST_F(TunableTest, ConfigImmutableAndOnTune) {
  int calls = 0;
  TunableConfig mutableConfig{.onTune =
                                  [&](detail::TunableBase&, ComplexTunable*) {
                                    ++calls;
                                  }};
  TunableConfig immutableConfig{.isMutable = false,
                                .onTune = mutableConfig.onTune};
  TunableInt32 mutableTunable{0, mutableConfig};
  TunableInt32 immutableTunable{5, immutableConfig};

  Tunables::Publish("mutable", mutableTunable);
  Tunables::Publish("immutable", immutableTunable);

  backend->SetInt32("/mutable", 1);
  backend->SetInt32("/immutable", 42);
  EXPECT_EQ(calls, 0);
  TunableRegistry::Update();

  EXPECT_EQ(mutableTunable.Get(), 1);
  EXPECT_EQ(immutableTunable.Get(), 5);
  EXPECT_EQ(calls, 1);
}

TEST_F(TunableTest, TunableConfigOptions) {
  int calls = 0;
  TunableConfig config{
      .properties = wpi::util::json::object("min", 0),
      .robust = true,
      .typeString = "UnitTestWidget",
      .isMutable = false,
      .onTune =
          [&](detail::TunableBase&, ComplexTunable*) {
            ++calls;
          },
      .alwaysGet = true};
  class InspectableInt : public TunableInt32 {
   public:
    using TunableInt32::TunableInt32;

    uint32_t GetUid() const { return GetTunableUid(); }
  };
  InspectableInt tunable{1, config};
  auto info = TunableRegistry::GetTunable(tunable.GetUid());

  ASSERT_TRUE(info);
  ASSERT_NE(info.config, nullptr);
  EXPECT_TRUE(info.config->robust);
  EXPECT_EQ(info.config->properties.at("min"), 0);
  ASSERT_TRUE(info.config->typeString.has_value());
  EXPECT_EQ(info.config->typeString.value(), "UnitTestWidget");
  EXPECT_FALSE(info.config->isMutable);
  EXPECT_TRUE(info.config->alwaysGet);

  Tunables::Publish("configured", tunable);
  backend->SetInt32("/configured", 2);
  TunableRegistry::Update();

  EXPECT_EQ(tunable.Get(), 1);
  EXPECT_EQ(calls, 0);

  info.config->onTune(tunable, nullptr);
  EXPECT_EQ(calls, 1);
}

TEST_F(TunableTest, TunablesGetTableFacade) {
  Tunable<double> tunable;
  auto table = Tunables::GetTable("arm");

  table.Publish("speed", tunable);
  backend->SetDouble("/arm/speed", 2.0);
  TunableRegistry::Update();

  EXPECT_EQ(tunable.Get(), 2.0);
}

TEST_F(TunableTest, TablePathsRouteAndRemove) {
  auto childBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("/child", childBackend);

  EXPECT_EQ(Tunables::GetTable().GetPath(), "/");
  EXPECT_EQ(Tunables::GetTable("drive").GetPath(), "/drive/");
  EXPECT_EQ(Tunables::GetTable("drive").GetTable("left").GetPath(),
            "/drive/left/");

  TunableDouble root{1.0};
  TunableDouble child{2.0};
  Tunables::Publish("root", root);
  Tunables::Publish("child/value", child);

  backend->SetDouble("/root", 3.0);
  childBackend->SetDouble("/child/value", 4.0);
  TunableRegistry::Update();
  EXPECT_EQ(root.Get(), 3.0);
  EXPECT_EQ(child.Get(), 4.0);
  EXPECT_THROW(backend->SetDouble("/child/value", 5.0), std::runtime_error);

  Tunables::Remove("child/value");
  EXPECT_THROW(childBackend->SetDouble("/child/value", 6.0),
               std::runtime_error);
}

TEST_F(TunableTest, RegisterBackendMigratesExistingMatchingTunables) {
  TunableDouble root{1.0};
  TunableDouble child{2.0};
  Tunables::Publish("root", root);
  Tunables::Publish("child/value", child);

  backend->SetDouble("/root", 3.0);
  backend->SetDouble("/child/value", 4.0);
  TunableRegistry::Update();
  EXPECT_EQ(root.Get(), 3.0);
  EXPECT_EQ(child.Get(), 4.0);

  auto childBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("/child", childBackend);

  EXPECT_THROW(backend->SetDouble("/child/value", 5.0), std::runtime_error);
  childBackend->SetDouble("/child/value", 6.0);
  backend->SetDouble("/root", 7.0);
  TunableRegistry::Update();

  EXPECT_EQ(root.Get(), 7.0);
  EXPECT_EQ(child.Get(), 6.0);
}

TEST_F(TunableTest, RegisterBackendReplacementMigratesExistingTunables) {
  TunableDouble tunable{1.0};
  Tunables::Publish("value", tunable);

  auto replacementBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("", replacementBackend);

  EXPECT_THROW(backend->SetDouble("/value", 2.0), std::runtime_error);
  replacementBackend->SetDouble("/value", 3.0);
  TunableRegistry::Update();

  EXPECT_EQ(tunable.Get(), 3.0);
}

TEST_F(TunableTest, RegisterBackendMigratesComplexTunable) {
  MemberComplex complex;
  Tunables::Publish("child/complex", complex);

  auto childBackend = std::make_shared<MockTunableBackend>();
  TunableRegistry::RegisterBackend("/child", childBackend);

  EXPECT_THROW(backend->SetInt32("/child/complex/gain", 2),
               std::runtime_error);
  childBackend->SetInt32("/child/complex/gain", 4);
  childBackend->SetStruct<TestStruct>("/child/complex/point", {5, 6});
  TunableRegistry::Update();

  EXPECT_EQ(complex.gain, 4);
  EXPECT_EQ(complex.point.a, 5);
  EXPECT_EQ(complex.point.b, 6);
  EXPECT_EQ(complex.updateCount, 1);
}

TEST_F(TunableTest, MockBackendRemovePrefixReturnsMatchingTunables) {
  TunableDouble root{1.0};
  TunableDouble child{2.0};
  Tunables::Publish("root", root);
  Tunables::Publish("child/value", child);

  auto removed = backend->RemovePrefix("/child");

  ASSERT_EQ(removed.size(), 1u);
  EXPECT_EQ(removed[0].path, "/child/value");
  backend->SetDouble("/root", 3.0);
  EXPECT_THROW(backend->SetDouble("/child/value", 4.0), std::runtime_error);
  TunableRegistry::Update();
  EXPECT_EQ(root.Get(), 3.0);
  EXPECT_EQ(child.Get(), 2.0);
}

TEST_F(TunableTest, ComplexTunablePublishesMembersAndUpdates) {
  MemberComplex complex;
  Tunables::Publish("complex", complex);

  backend->SetInt32("/complex/gain", 10);
  backend->SetStruct<TestStruct>("/complex/point", {11, 12});
  TunableRegistry::Update();

  EXPECT_EQ(complex.gain, 10);
  EXPECT_EQ(complex.point.a, 11);
  EXPECT_EQ(complex.point.b, 12);
  EXPECT_EQ(complex.updateCount, 1);
}

TEST_F(TunableTest, CustomTunable) {
  Tunable<CustomType> tunable;

  tunable.Set(CustomType{42});
  auto val = tunable.Get();
  EXPECT_EQ(val.val, 42);

  tunable = CustomType{63};
  val = tunable;
  EXPECT_EQ(val.val, 63);

  Tunables::Publish("testCustom", tunable);
  backend->SetInt32("/testCustom", 84);
  TunableRegistry::Update();
  val = tunable.Get();
  EXPECT_EQ(val.val, 84);
}

TEST_F(TunableTest, StructTunable) {
  Tunable<TestStruct> tunable;

  tunable.Set({1, 2});
  auto val = tunable.Get();
  EXPECT_EQ(val.a, 1);
  EXPECT_EQ(val.b, 2);

  tunable = {2, 3};
  val = tunable;
  EXPECT_EQ(val.a, 2);
  EXPECT_EQ(val.b, 3);

  Tunables::Publish("testStruct", tunable);
  backend->SetStruct<TestStruct>("/testStruct", {3, 4});
  TunableRegistry::Update();
  val = tunable.Get();
  EXPECT_EQ(val.a, 3);
  EXPECT_EQ(val.b, 4);
}

TEST_F(TunableTest, StructVectorTunable) {
  Tunable<std::vector<TestStruct>> tunable;

  tunable.Set({{1, 2}, {3, 4}});
  auto val = tunable.Get();
  EXPECT_EQ(val.size(), 2u);
  EXPECT_EQ(val[0].a, 1);
  EXPECT_EQ(val[0].b, 2);
  EXPECT_EQ(val[1].a, 3);
  EXPECT_EQ(val[1].b, 4);

  tunable = {{2, 3}, {4, 5}, {6, 7}};
  val = tunable;
  EXPECT_EQ(val.size(), 3u);
  EXPECT_EQ(val[0].a, 2);
  EXPECT_EQ(val[0].b, 3);
  EXPECT_EQ(val[1].a, 4);
  EXPECT_EQ(val[1].b, 5);
  EXPECT_EQ(val[2].a, 6);
  EXPECT_EQ(val[2].b, 7);

  Tunables::Publish("testStructVector", tunable);
  backend->SetStructVector<TestStruct>("/testStructVector", {{{5, 6}, {7, 8}}});
  TunableRegistry::Update();
  val = tunable.Get();
  EXPECT_EQ(val.size(), 2u);
  EXPECT_EQ(val[0].a, 5);
  EXPECT_EQ(val[0].b, 6);
  EXPECT_EQ(val[1].a, 7);
  EXPECT_EQ(val[1].b, 8);
}

TEST_F(TunableTest, StructArrayTunable) {
  Tunable<std::array<TestStruct, 2>> tunable;

  tunable.Set({{{1, 2}, {3, 4}}});
  auto val = tunable.Get();
  EXPECT_EQ(val.size(), 2u);
  EXPECT_EQ(val[0].a, 1);
  EXPECT_EQ(val[0].b, 2);
  EXPECT_EQ(val[1].a, 3);
  EXPECT_EQ(val[1].b, 4);

  tunable = {{{2, 3}, {4, 5}}};
  val = tunable;
  EXPECT_EQ(val.size(), 2u);
  EXPECT_EQ(val[0].a, 2);
  EXPECT_EQ(val[0].b, 3);
  EXPECT_EQ(val[1].a, 4);
  EXPECT_EQ(val[1].b, 5);

  Tunables::Publish("testStructArray", tunable);
  backend->SetStructVector<TestStruct>("/testStructArray", {{{5, 6}, {7, 8}}});
  TunableRegistry::Update();
  val = tunable.Get();
  EXPECT_EQ(val.size(), 2u);
  EXPECT_EQ(val[0].a, 5);
  EXPECT_EQ(val[0].b, 6);
  EXPECT_EQ(val[1].a, 7);
  EXPECT_EQ(val[1].b, 8);
}
