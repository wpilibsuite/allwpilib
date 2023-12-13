// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/SpanMatcher.h>
#include <wpi/struct/Struct.h>

#include "networktables/NetworkTableInstance.h"
#include "networktables/StructArrayTopic.h"
#include "networktables/StructTopic.h"

namespace {
struct Inner {
  int a;
  int b;
};

struct Outer {
  Inner inner;
  int c;
};

struct Inner2 {
  int a;
  int b;
};

struct Outer2 {
  Inner2 inner;
  int c;
};
}  // namespace

template <>
struct wpi::Struct<Inner> {
  static constexpr std::string_view GetTypeString() { return "struct:Inner"; }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() { return "int32 a; int32 b"; }

  static Inner Unpack(std::span<const uint8_t> data) {
    return {wpi::UnpackStruct<int32_t, 0>(data),
            wpi::UnpackStruct<int32_t, 4>(data)};
  }
  static void Pack(std::span<uint8_t> data, const Inner& value) {
    wpi::PackStruct<0>(data, value.a);
    wpi::PackStruct<4>(data, value.b);
  }
};

template <>
struct wpi::Struct<Outer> {
  static constexpr std::string_view GetTypeString() { return "struct:Outer"; }
  static constexpr size_t GetSize() { return wpi::GetStructSize<Inner>() + 4; }
  static constexpr std::string_view GetSchema() {
    return "Inner inner; int32 c";
  }

  static Outer Unpack(std::span<const uint8_t> data) {
    constexpr size_t innerSize = wpi::GetStructSize<Inner>();
    return {wpi::UnpackStruct<Inner, 0>(data),
            wpi::UnpackStruct<int32_t, innerSize>(data)};
  }
  static void Pack(std::span<uint8_t> data, const Outer& value) {
    constexpr size_t innerSize = wpi::GetStructSize<Inner>();
    wpi::PackStruct<0>(data, value.inner);
    wpi::PackStruct<innerSize>(data, value.c);
  }
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<Inner>(fn);
  }
};

template <>
struct wpi::Struct<Inner2> {
  static std::string_view GetTypeString() { return "struct:Inner2"; }
  static size_t GetSize() { return 8; }
  static std::string_view GetSchema() { return "int32 a; int32 b"; }

  static Inner2 Unpack(std::span<const uint8_t> data) {
    return {wpi::UnpackStruct<int32_t, 0>(data),
            wpi::UnpackStruct<int32_t, 4>(data)};
  }
  static void Pack(std::span<uint8_t> data, const Inner2& value) {
    wpi::PackStruct<0>(data, value.a);
    wpi::PackStruct<4>(data, value.b);
  }
};

template <>
struct wpi::Struct<Outer2> {
  static std::string_view GetTypeString() { return "struct:Outer2"; }
  static size_t GetSize() { return wpi::GetStructSize<Inner>() + 4; }
  static std::string_view GetSchema() { return "Inner2 inner; int32 c"; }

  static Outer2 Unpack(std::span<const uint8_t> data) {
    size_t innerSize = wpi::GetStructSize<Inner2>();
    return {wpi::UnpackStruct<Inner2, 0>(data),
            wpi::UnpackStruct<int32_t>(data.subspan(innerSize))};
  }
  static void Pack(std::span<uint8_t> data, const Outer2& value) {
    size_t innerSize = wpi::GetStructSize<Inner2>();
    wpi::PackStruct<0>(data, value.inner);
    wpi::PackStruct(data.subspan(innerSize), value.c);
  }
  static void ForEachNested(
      std::invocable<std::string_view, std::string_view> auto fn) {
    wpi::ForEachStructSchema<Inner2>(fn);
  }
};

namespace nt {

class StructTest : public ::testing::Test {
 public:
  StructTest() { inst = nt::NetworkTableInstance::Create(); }
  ~StructTest() { nt::NetworkTableInstance::Destroy(inst); }

  nt::NetworkTableInstance inst;
};

TEST_F(StructTest, InnerConstexpr) {
  nt::StructTopic<Inner> topic = inst.GetStructTopic<Inner>("inner");
  nt::StructPublisher<Inner> pub = topic.Publish();
  nt::StructSubscriber<Inner> sub = topic.Subscribe({});

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
  nt::StructTopic<Inner2> topic = inst.GetStructTopic<Inner2>("inner2");
  nt::StructPublisher<Inner2> pub = topic.Publish();
  nt::StructSubscriber<Inner2> sub = topic.Subscribe({});

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
  nt::StructTopic<Outer> topic = inst.GetStructTopic<Outer>("outer");
  nt::StructPublisher<Outer> pub = topic.Publish();
  nt::StructSubscriber<Outer> sub = topic.Subscribe({});

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
  nt::StructTopic<Outer2> topic = inst.GetStructTopic<Outer2>("outer2");
  nt::StructPublisher<Outer2> pub = topic.Publish();
  nt::StructSubscriber<Outer2> sub = topic.Subscribe({});

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
  nt::StructArrayTopic<Inner> topic = inst.GetStructArrayTopic<Inner>("innerA");
  nt::StructArrayPublisher<Inner> pub = topic.Publish();
  nt::StructArraySubscriber<Inner> sub = topic.Subscribe({});

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
  nt::StructArrayTopic<Inner2> topic =
      inst.GetStructArrayTopic<Inner2>("innerA2");
  nt::StructArrayPublisher<Inner2> pub = topic.Publish();
  nt::StructArraySubscriber<Inner2> sub = topic.Subscribe({});

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

}  // namespace nt
