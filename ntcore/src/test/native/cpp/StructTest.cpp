// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StructArrayTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/util/SpanMatcher.hpp"
#include "wpi/util/struct/Struct.hpp"

namespace {
struct Inner {
  int a = 0;
  int b = 0;
};

struct Outer {
  Inner inner;
  int c = 0;
};

struct Inner2 {
  int a = 0;
  int b = 0;
};

struct Outer2 {
  Inner2 inner;
  int c = 0;
};

struct ThingA {
  int x = 0;
};

struct ThingB {
  int x = 0;
};

struct Info1 {
  int info = 0;
};
}  // namespace

template <>
struct wpi::util::Struct<Inner> {
  static constexpr std::string_view GetTypeName() { return "Inner"; }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() { return "int32 a; int32 b"; }

  static Inner Unpack(std::span<const uint8_t> data) {
    return {wpi::util::UnpackStruct<int32_t, 0>(data),
            wpi::util::UnpackStruct<int32_t, 4>(data)};
  }
  static void Pack(std::span<uint8_t> data, const Inner& value) {
    wpi::util::PackStruct<0>(data, value.a);
    wpi::util::PackStruct<4>(data, value.b);
  }
};

template <>
struct wpi::util::Struct<Outer> {
  static constexpr std::string_view GetTypeName() { return "Outer"; }
  static constexpr size_t GetSize() { return wpi::util::GetStructSize<Inner>() + 4; }
  static constexpr std::string_view GetSchema() {
    return "Inner inner; int32 c";
  }

  static Outer Unpack(std::span<const uint8_t> data) {
    constexpr size_t innerSize = wpi::util::GetStructSize<Inner>();
    return {wpi::util::UnpackStruct<Inner, 0>(data),
            wpi::util::UnpackStruct<int32_t, innerSize>(data)};
  }
  static void Pack(std::span<uint8_t> data, const Outer& value) {
    constexpr size_t innerSize = wpi::util::GetStructSize<Inner>();
    wpi::util::PackStruct<0>(data, value.inner);
    wpi::util::PackStruct<innerSize>(data, value.c);
  }
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<Inner>(fn);
  }
};

template <>
struct wpi::util::Struct<Inner2> {
  static std::string_view GetTypeName() { return "Inner2"; }
  static size_t GetSize() { return 8; }
  static std::string_view GetSchema() { return "int32 a; int32 b"; }

  static Inner2 Unpack(std::span<const uint8_t> data) {
    return {wpi::util::UnpackStruct<int32_t, 0>(data),
            wpi::util::UnpackStruct<int32_t, 4>(data)};
  }
  static void Pack(std::span<uint8_t> data, const Inner2& value) {
    wpi::util::PackStruct<0>(data, value.a);
    wpi::util::PackStruct<4>(data, value.b);
  }
};

template <>
struct wpi::util::Struct<Outer2> {
  static std::string_view GetTypeName() { return "Outer2"; }
  static size_t GetSize() { return wpi::util::GetStructSize<Inner>() + 4; }
  static std::string_view GetSchema() { return "Inner2 inner; int32 c"; }

  static Outer2 Unpack(std::span<const uint8_t> data) {
    size_t innerSize = wpi::util::GetStructSize<Inner2>();
    return {wpi::util::UnpackStruct<Inner2, 0>(data),
            wpi::util::UnpackStruct<int32_t>(data.subspan(innerSize))};
  }
  static void Pack(std::span<uint8_t> data, const Outer2& value) {
    size_t innerSize = wpi::util::GetStructSize<Inner2>();
    wpi::util::PackStruct<0>(data, value.inner);
    wpi::util::PackStruct(data.subspan(innerSize), value.c);
  }
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::util::ForEachStructSchema<Inner2>(fn);
  }
};

template <>
struct wpi::util::Struct<ThingA> {
  static constexpr std::string_view GetTypeName() { return "ThingA"; }
  static constexpr size_t GetSize() { return 1; }
  static constexpr std::string_view GetSchema() { return "uint8 value"; }
  static ThingA Unpack(std::span<const uint8_t> data) {
    return ThingA{.x = data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingA& value) {
    data[0] = value.x;
  }
};

template <>
struct wpi::util::Struct<ThingB, Info1> {
  static constexpr std::string_view GetTypeName(const Info1&) {
    return "ThingB";
  }
  static constexpr size_t GetSize(const Info1&) { return 1; }
  static constexpr std::string_view GetSchema(const Info1&) {
    return "uint8 value";
  }
  static ThingB Unpack(std::span<const uint8_t> data, const Info1&) {
    return ThingB{.x = data[0]};
  }
  static void Pack(std::span<uint8_t> data, const ThingB& value, const Info1&) {
    data[0] = value.x;
  }
};

namespace wpi::nt {

class StructTest : public ::testing::Test {
 public:
  StructTest() { inst = wpi::nt::NetworkTableInstance::Create(); }
  ~StructTest() override { wpi::nt::NetworkTableInstance::Destroy(inst); }

  wpi::nt::NetworkTableInstance inst;
};

TEST_F(StructTest, InnerConstexpr) {
  wpi::nt::StructTopic<Inner> topic = inst.GetStructTopic<Inner>("inner");
  wpi::nt::StructPublisher<Inner> pub = topic.Publish();
  wpi::nt::StructSubscriber<Inner> sub = topic.Subscribe({});

  ASSERT_EQ(topic.GetTypeString(), "struct:Inner");

  pub.SetDefault({0, 1});
  Inner val = sub.Get();
  ASSERT_EQ(val.a, 0);
  ASSERT_EQ(val.b, 1);

  pub.Set({1, 2});
  auto atomicVal = sub.GetAtomic();
  ASSERT_EQ(atomicVal.value.a, 1);
  ASSERT_EQ(atomicVal.value.b, 2);

  Inner val2;
  sub.GetInto(&val2);
  ASSERT_EQ(val2.a, 1);
  ASSERT_EQ(val2.b, 2);

  auto vals = sub.ReadQueue();
  ASSERT_EQ(vals.size(), 1u);
  ASSERT_EQ(vals[0].value.a, 1);
  ASSERT_EQ(vals[0].value.b, 2);
}

TEST_F(StructTest, InnerNonconstexpr) {
  wpi::nt::StructTopic<Inner2> topic = inst.GetStructTopic<Inner2>("inner2");
  wpi::nt::StructPublisher<Inner2> pub = topic.Publish();
  wpi::nt::StructSubscriber<Inner2> sub = topic.Subscribe({});

  ASSERT_EQ(topic.GetTypeString(), "struct:Inner2");

  pub.SetDefault({0, 1});
  Inner2 val = sub.Get();
  ASSERT_EQ(val.a, 0);
  ASSERT_EQ(val.b, 1);

  pub.Set({1, 2});
  auto atomicVal = sub.GetAtomic();
  ASSERT_EQ(atomicVal.value.a, 1);
  ASSERT_EQ(atomicVal.value.b, 2);

  Inner2 val2;
  sub.GetInto(&val2);
  ASSERT_EQ(val2.a, 1);
  ASSERT_EQ(val2.b, 2);

  auto vals = sub.ReadQueue();
  ASSERT_EQ(vals.size(), 1u);
  ASSERT_EQ(vals[0].value.a, 1);
  ASSERT_EQ(vals[0].value.b, 2);
}

TEST_F(StructTest, OuterConstexpr) {
  wpi::nt::StructTopic<Outer> topic = inst.GetStructTopic<Outer>("outer");
  wpi::nt::StructPublisher<Outer> pub = topic.Publish();
  wpi::nt::StructSubscriber<Outer> sub = topic.Subscribe({});

  ASSERT_EQ(topic.GetTypeString(), "struct:Outer");

  pub.SetDefault({{0, 1}, 2});
  Outer val = sub.Get();
  ASSERT_EQ(val.inner.a, 0);
  ASSERT_EQ(val.inner.b, 1);
  ASSERT_EQ(val.c, 2);

  pub.Set({{1, 2}, 3});
  auto atomicVal = sub.GetAtomic();
  ASSERT_EQ(atomicVal.value.inner.a, 1);
  ASSERT_EQ(atomicVal.value.inner.b, 2);
  ASSERT_EQ(atomicVal.value.c, 3);

  Outer val2;
  sub.GetInto(&val2);
  ASSERT_EQ(val2.inner.a, 1);
  ASSERT_EQ(val2.inner.b, 2);
  ASSERT_EQ(val2.c, 3);

  auto vals = sub.ReadQueue();
  ASSERT_EQ(vals.size(), 1u);
  ASSERT_EQ(vals[0].value.inner.a, 1);
  ASSERT_EQ(vals[0].value.inner.b, 2);
  ASSERT_EQ(vals[0].value.c, 3);
}

TEST_F(StructTest, OuterNonconstexpr) {
  wpi::nt::StructTopic<Outer2> topic = inst.GetStructTopic<Outer2>("outer2");
  wpi::nt::StructPublisher<Outer2> pub = topic.Publish();
  wpi::nt::StructSubscriber<Outer2> sub = topic.Subscribe({});

  ASSERT_EQ(topic.GetTypeString(), "struct:Outer2");

  pub.SetDefault({{0, 1}, 2});
  Outer2 val = sub.Get();
  ASSERT_EQ(val.inner.a, 0);
  ASSERT_EQ(val.inner.b, 1);
  ASSERT_EQ(val.c, 2);

  pub.Set({{1, 2}, 3});
  auto atomicVal = sub.GetAtomic();
  ASSERT_EQ(atomicVal.value.inner.a, 1);
  ASSERT_EQ(atomicVal.value.inner.b, 2);
  ASSERT_EQ(atomicVal.value.c, 3);

  Outer2 val2;
  sub.GetInto(&val2);
  ASSERT_EQ(val2.inner.a, 1);
  ASSERT_EQ(val2.inner.b, 2);
  ASSERT_EQ(val2.c, 3);

  auto vals = sub.ReadQueue();
  ASSERT_EQ(vals.size(), 1u);
  ASSERT_EQ(vals[0].value.inner.a, 1);
  ASSERT_EQ(vals[0].value.inner.b, 2);
  ASSERT_EQ(vals[0].value.c, 3);
}

TEST_F(StructTest, InnerArrayConstexpr) {
  wpi::nt::StructArrayTopic<Inner> topic = inst.GetStructArrayTopic<Inner>("innerA");
  wpi::nt::StructArrayPublisher<Inner> pub = topic.Publish();
  wpi::nt::StructArraySubscriber<Inner> sub = topic.Subscribe({});

  ASSERT_EQ(topic.GetTypeString(), "struct:Inner[]");

  pub.SetDefault({{{0, 1}}});
  auto val = sub.Get();
  ASSERT_EQ(val.size(), 1u);
  ASSERT_EQ(val[0].a, 0);
  ASSERT_EQ(val[0].b, 1);

  pub.Set({{{1, 2}}});
  auto atomicVal = sub.GetAtomic();
  ASSERT_EQ(atomicVal.value.size(), 1u);
  ASSERT_EQ(atomicVal.value[0].a, 1);
  ASSERT_EQ(atomicVal.value[0].b, 2);

  auto vals = sub.ReadQueue();
  ASSERT_EQ(vals.size(), 1u);
  ASSERT_EQ(vals[0].value.size(), 1u);
  ASSERT_EQ(vals[0].value[0].a, 1);
  ASSERT_EQ(vals[0].value[0].b, 2);
}

TEST_F(StructTest, InnerArrayNonconstexpr) {
  wpi::nt::StructArrayTopic<Inner2> topic =
      inst.GetStructArrayTopic<Inner2>("innerA2");
  wpi::nt::StructArrayPublisher<Inner2> pub = topic.Publish();
  wpi::nt::StructArraySubscriber<Inner2> sub = topic.Subscribe({});

  ASSERT_EQ(topic.GetTypeString(), "struct:Inner2[]");

  pub.SetDefault({{{0, 1}}});
  auto val = sub.Get();
  ASSERT_EQ(val.size(), 1u);
  ASSERT_EQ(val[0].a, 0);
  ASSERT_EQ(val[0].b, 1);

  pub.Set({{{1, 2}}});
  auto atomicVal = sub.GetAtomic();
  ASSERT_EQ(atomicVal.value.size(), 1u);
  ASSERT_EQ(atomicVal.value[0].a, 1);
  ASSERT_EQ(atomicVal.value[0].b, 2);

  auto vals = sub.ReadQueue();
  ASSERT_EQ(vals.size(), 1u);
  ASSERT_EQ(vals[0].value.size(), 1u);
  ASSERT_EQ(vals[0].value[0].a, 1);
  ASSERT_EQ(vals[0].value[0].b, 2);
}

TEST_F(StructTest, StructA) {
  wpi::nt::StructTopic<ThingA> topic = inst.GetStructTopic<ThingA>("a");
  wpi::nt::StructPublisher<ThingA> pub = topic.Publish();
  wpi::nt::StructPublisher<ThingA> pub2 = topic.PublishEx({{}});
  wpi::nt::StructSubscriber<ThingA> sub = topic.Subscribe({});
  wpi::nt::StructEntry<ThingA> entry = topic.GetEntry({});
  pub.SetDefault({});
  pub.Set({}, 5);
  sub.Get();
  sub.Get({});
  sub.GetAtomic();
  sub.GetAtomic({});
  entry.SetDefault({});
  entry.Set({}, 6);
  entry.Get({});
}

TEST_F(StructTest, StructArrayA) {
  wpi::nt::StructArrayTopic<ThingA> topic = inst.GetStructArrayTopic<ThingA>("a");
  wpi::nt::StructArrayPublisher<ThingA> pub = topic.Publish();
  wpi::nt::StructArrayPublisher<ThingA> pub2 = topic.PublishEx({{}});
  wpi::nt::StructArraySubscriber<ThingA> sub = topic.Subscribe({});
  wpi::nt::StructArrayEntry<ThingA> entry = topic.GetEntry({});
  pub.SetDefault({{ThingA{}, ThingA{}}});
  pub.Set({{ThingA{}, ThingA{}}}, 5);
  sub.Get();
  sub.Get({});
  sub.GetAtomic();
  sub.GetAtomic({});
  entry.SetDefault({{ThingA{}, ThingA{}}});
  entry.Set({{ThingA{}, ThingA{}}}, 6);
  entry.Get({});
}

TEST_F(StructTest, StructFixedArrayA) {
  wpi::nt::StructTopic<std::array<ThingA, 2>> topic =
      inst.GetStructTopic<std::array<ThingA, 2>>("a");
  wpi::nt::StructPublisher<std::array<ThingA, 2>> pub = topic.Publish();
  wpi::nt::StructPublisher<std::array<ThingA, 2>> pub2 = topic.PublishEx({{}});
  wpi::nt::StructSubscriber<std::array<ThingA, 2>> sub = topic.Subscribe({});
  wpi::nt::StructEntry<std::array<ThingA, 2>> entry = topic.GetEntry({});
  std::array<ThingA, 2> arr;
  pub.SetDefault(arr);
  pub.Set(arr, 5);
  sub.Get();
  sub.Get(arr);
  sub.GetAtomic();
  sub.GetAtomic(arr);
  entry.SetDefault(arr);
  entry.Set(arr, 6);
  entry.Get(arr);
}

TEST_F(StructTest, StructB) {
  Info1 info;
  wpi::nt::StructTopic<ThingB, Info1> topic =
      inst.GetStructTopic<ThingB, Info1>("b", info);
  wpi::nt::StructPublisher<ThingB, Info1> pub = topic.Publish();
  wpi::nt::StructPublisher<ThingB, Info1> pub2 = topic.PublishEx({{}});
  wpi::nt::StructSubscriber<ThingB, Info1> sub = topic.Subscribe({});
  wpi::nt::StructEntry<ThingB, Info1> entry = topic.GetEntry({});
  pub.SetDefault({});
  pub.Set({}, 5);
  sub.Get();
  sub.Get({});
  sub.GetAtomic();
  sub.GetAtomic({});
  entry.SetDefault({});
  entry.Set({}, 6);
  entry.Get({});
}

TEST_F(StructTest, StructArrayB) {
  Info1 info;
  wpi::nt::StructArrayTopic<ThingB, Info1> topic =
      inst.GetStructArrayTopic<ThingB, Info1>("b", info);
  wpi::nt::StructArrayPublisher<ThingB, Info1> pub = topic.Publish();
  wpi::nt::StructArrayPublisher<ThingB, Info1> pub2 = topic.PublishEx({{}});
  wpi::nt::StructArraySubscriber<ThingB, Info1> sub = topic.Subscribe({});
  wpi::nt::StructArrayEntry<ThingB, Info1> entry = topic.GetEntry({});
  pub.SetDefault({{ThingB{}, ThingB{}}});
  pub.Set({{ThingB{}, ThingB{}}}, 5);
  sub.Get();
  sub.Get({});
  sub.GetAtomic();
  sub.GetAtomic({});
  entry.SetDefault({{ThingB{}, ThingB{}}});
  entry.Set({{ThingB{}, ThingB{}}}, 6);
  entry.Get({});
}

TEST_F(StructTest, StructFixedArrayB) {
  Info1 info;
  wpi::nt::StructTopic<std::array<ThingB, 2>, Info1> topic =
      inst.GetStructTopic<std::array<ThingB, 2>, Info1>("b", info);
  wpi::nt::StructPublisher<std::array<ThingB, 2>, Info1> pub = topic.Publish();
  wpi::nt::StructPublisher<std::array<ThingB, 2>, Info1> pub2 =
      topic.PublishEx({{}});
  wpi::nt::StructSubscriber<std::array<ThingB, 2>, Info1> sub = topic.Subscribe({});
  wpi::nt::StructEntry<std::array<ThingB, 2>, Info1> entry = topic.GetEntry({});
  std::array<ThingB, 2> arr;
  pub.SetDefault(arr);
  pub.Set(arr, 5);
  sub.Get();
  sub.Get(arr);
  sub.GetAtomic();
  sub.GetAtomic(arr);
  entry.SetDefault(arr);
  entry.Set(arr, 6);
  entry.Get(arr);
}

}  // namespace wpi::nt
