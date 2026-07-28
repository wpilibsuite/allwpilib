// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/struct/Struct.hpp"

#include <stdint.h>

#include <array>
#include <string>
#include <vector>

#include <gtest/gtest.h>

using namespace wpi::util;

static_assert(StructSerializable<int>);
static_assert(StructSerializable<std::array<int, 5>>);
static_assert(!StructSerializable<std::vector<int>>);
static_assert(!StructSerializable<std::string>);

namespace {
struct TestStruct {
  int32_t a;
  int32_t b;
};
}  // namespace

template <>
struct wpi::util::Struct<TestStruct> {
  static constexpr std::string_view GetTypeName() { return "TestStruct"; }
  static constexpr size_t GetSize() { return 8; }
  static constexpr std::string_view GetSchema() { return "int32 a;int32 b"; }
  static TestStruct Unpack(std::span<const uint8_t> data) {
    return {wpi::util::UnpackStruct<int32_t, 0>(data),
            wpi::util::UnpackStruct<int32_t, 4>(data)};
  }
  static void Pack(std::span<uint8_t> data, TestStruct value) {
    wpi::util::PackStruct<0>(data, value.a);
    wpi::util::PackStruct<4>(data, value.b);
  }
};

TEST(StructTest, Basic) {
  constexpr auto typeName = GetStructTypeName<TestStruct>();
  EXPECT_EQ(typeName, "TestStruct");

  constexpr auto typeString = GetStructTypeString<TestStruct>();
  EXPECT_EQ(typeString, "struct:TestStruct");

  constexpr auto schema = GetStructSchema<TestStruct>();
  EXPECT_EQ(schema, "int32 a;int32 b");

  constexpr auto arrayTypeName = MakeStructArrayTypeName<TestStruct, 5>();
  EXPECT_EQ(arrayTypeName, "TestStruct[5]");

  constexpr auto arrayTypeString = MakeStructArrayTypeString<TestStruct, 5>();
  EXPECT_EQ(arrayTypeString, "struct:TestStruct[5]");

  TestStruct s{12345, 67890};
  std::array<uint8_t, 8> data;
  Struct<TestStruct>::Pack(data, s);
  auto s2 = Struct<TestStruct>::Unpack(data);
  EXPECT_EQ(s.a, s2.a);
  EXPECT_EQ(s.b, s2.b);
}
