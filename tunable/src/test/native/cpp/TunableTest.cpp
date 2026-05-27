// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/tunable/Tunable.hpp"

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "wpi/tunable/MockTunableBackend.hpp"
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

TEST_F(TunableTest, TunablesGetTableFacade) {
  Tunable<double> tunable;
  auto table = Tunables::GetTable("arm");

  table.Publish("speed", tunable);
  backend->SetDouble("/arm/speed", 2.0);
  TunableRegistry::Update();

  EXPECT_EQ(tunable.Get(), 2.0);
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
