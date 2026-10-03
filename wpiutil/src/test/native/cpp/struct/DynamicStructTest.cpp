// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/struct/DynamicStruct.hpp"

#include <stdint.h>

#include <cstring>
#include <limits>
#include <string>
#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using namespace wpi::util;

class DynamicStructTest {
 protected:
  StructDescriptorDatabase db;
  std::string err;
};

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest Empty",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "", &err);
  REQUIRE(desc);
  REQUIRE(desc->GetName() == "test");
  REQUIRE(desc->GetSchema() == "");
  REQUIRE(desc->GetFields().empty());
  REQUIRE(desc->IsValid());
  REQUIRE(desc->GetSize() == 0u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest NestedStruct",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int32 a", &err);
  REQUIRE(desc);
  REQUIRE(desc->IsValid());
  auto desc2 = db.Add("test2", "test a", &err);
  REQUIRE(desc2);
  REQUIRE(desc2->IsValid());
  REQUIRE(desc2->GetSize() == 4u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest DelayedValid",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "foo a", &err);
  REQUIRE(desc);
  REQUIRE_FALSE(desc->IsValid());
  auto desc2 = db.Add("test2", "foo a;foo b;", &err);
  REQUIRE(desc2);
  REQUIRE_FALSE(desc2->IsValid());
  auto desc3 = db.Add("test3", "foo a[2]", &err);
  REQUIRE(desc3);
  REQUIRE_FALSE(desc3->IsValid());
  auto desc4 = db.Add("foo", "int32 a", &err);
  REQUIRE(desc4);
  REQUIRE(desc4->IsValid());
  REQUIRE(desc->IsValid());
  REQUIRE(desc->GetSize() == 4u);
  REQUIRE(desc2->IsValid());
  REQUIRE(desc2->GetSize() == 8u);
  REQUIRE(desc3->IsValid());
  REQUIRE(desc3->GetSize() == 8u);
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest ReuseNestedStructDelayed",
                 "[wpiutil][struct]") {
  auto desc2 = db.Add("test2", "test a;test b;", &err);
  auto desc = db.Add("test", "int32 a; uint16 b; int16 c;", &err);
  REQUIRE(desc);
  REQUIRE(desc->IsValid());
  REQUIRE(desc2);
  REQUIRE(desc2->IsValid());
  REQUIRE(desc2->GetSize() == 16u);
  auto fields = desc2->GetFields();
  REQUIRE(fields[0].GetOffset() == 0u);
  REQUIRE(fields[0].GetName() == "a");
  REQUIRE(fields[1].GetOffset() == 8u);
  REQUIRE(fields[1].GetName() == "b");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest InvalidBitfield",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "float a:1", &err);
  CHECK_FALSE(desc);
  CHECK(err == "field a: type float cannot be bitfield");

  desc = db.Add("test", "double a:1", &err);
  CHECK_FALSE(desc);
  CHECK(err == "field a: type double cannot be bitfield");

  desc = db.Add("test", "foo a:1", &err);
  CHECK_FALSE(desc);
  CHECK(err == "field a: type foo cannot be bitfield");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest CircularStructReference",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "test a", &err);
  REQUIRE_FALSE(desc);
  REQUIRE(err == "field a: recursive struct reference");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest NestedCircularStructRef",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "foo a", &err);
  REQUIRE(desc);
  auto desc2 = db.Add("foo", "bar a", &err);
  REQUIRE(desc2);
  auto desc3 = db.Add("bar", "test a", &err);
  REQUIRE_FALSE(desc3);
  REQUIRE(err == "circular struct reference: bar <- foo <- test");

  // ok
  auto desc4 = db.Add("baz", "bar a", &err);
  REQUIRE(desc4);
  REQUIRE_FALSE(desc4->IsValid());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest NestedCircularStructRef2",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "foo a", &err);
  REQUIRE(desc);
  auto desc2 = db.Add("bar", "test a", &err);
  REQUIRE(desc2);
  auto desc3 = db.Add("foo", "bar a", &err);
  REQUIRE_FALSE(desc3);
  REQUIRE(err == "circular struct reference: foo <- test <- bar");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldBasic",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int32 a:2; uint32 b:30", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 4u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 2u);
  CHECK(fields[0].GetBitWidth() == 2u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0x3u);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 4u);
  CHECK(fields[1].GetBitWidth() == 30u);
  CHECK(fields[1].GetBitShift() == 2u);
  CHECK(fields[1].GetBitMask() == 0x3fffffffu);
  CHECK(fields[1].GetOffset() == 0u);
  CHECK(fields[1].GetSize() == 4u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldDiffType",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int32 a:2; int16 b:2", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 6u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 2u);
  CHECK(fields[0].GetBitWidth() == 2u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0x3u);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 4u);
  CHECK(fields[1].GetBitWidth() == 2u);
  CHECK(fields[1].GetBitShift() == 0u);
  CHECK(fields[1].GetBitMask() == 0x3u);
  CHECK(fields[1].GetOffset() == 4u);
  CHECK(fields[1].GetSize() == 2u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldOverflow",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int8 a:4; int8 b:5", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 2u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 2u);
  CHECK(fields[0].GetBitWidth() == 4u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0xfu);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 1u);
  CHECK(fields[1].GetBitWidth() == 5u);
  CHECK(fields[1].GetBitMask() == 0x1fu);
  CHECK(fields[1].GetBitShift() == 0u);
  CHECK(fields[1].GetOffset() == 1u);
  CHECK(fields[1].GetSize() == 1u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldBoolBegin8",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "bool a:1; int8 b:5", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 1u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 2u);
  CHECK(fields[0].GetBitWidth() == 1u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0x1u);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 1u);
  CHECK(fields[1].GetBitWidth() == 5u);
  CHECK(fields[1].GetBitMask() == 0x1fu);
  CHECK(fields[1].GetBitShift() == 1u);
  CHECK(fields[1].GetOffset() == 0u);
  CHECK(fields[1].GetSize() == 1u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldBoolBegin16",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "bool a:1; int16 b:5", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 3u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 2u);
  CHECK(fields[0].GetBitWidth() == 1u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0x1u);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 1u);
  CHECK(fields[1].GetBitWidth() == 5u);
  CHECK(fields[1].GetBitMask() == 0x1fu);
  CHECK(fields[1].GetBitShift() == 0u);
  CHECK(fields[1].GetOffset() == 1u);
  CHECK(fields[1].GetSize() == 2u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldBoolMid",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int16 a:2; bool b:1; bool c:1; uint16 d:5", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 2u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 4u);
  CHECK(fields[0].GetBitWidth() == 2u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0x3u);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 2u);
  CHECK(fields[1].GetBitWidth() == 1u);
  CHECK(fields[1].GetBitMask() == 0x1u);
  CHECK(fields[1].GetBitShift() == 2u);
  CHECK(fields[1].GetOffset() == 0u);
  CHECK(fields[1].GetSize() == 2u);
  CHECK(fields[2].GetBitWidth() == 1u);
  CHECK(fields[2].GetBitMask() == 0x1u);
  CHECK(fields[2].GetBitShift() == 3u);
  CHECK(fields[2].GetOffset() == 0u);
  CHECK(fields[2].GetSize() == 2u);
  CHECK(fields[3].GetBitWidth() == 5u);
  CHECK(fields[3].GetBitMask() == 0x1fu);
  CHECK(fields[3].GetBitShift() == 4u);
  CHECK(fields[3].GetOffset() == 0u);
  CHECK(fields[3].GetSize() == 2u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldBoolEnd",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int16 a:15; bool b:1", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 2u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 2u);
  CHECK(fields[0].GetBitWidth() == 15u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0x7fffu);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 2u);
  CHECK(fields[1].GetBitWidth() == 1u);
  CHECK(fields[1].GetBitMask() == 0x1u);
  CHECK(fields[1].GetBitShift() == 15u);
  CHECK(fields[1].GetOffset() == 0u);
  CHECK(fields[1].GetSize() == 2u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldBoolEnd2",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int16 a:16; bool b:1", &err);
  REQUIRE(desc);
  CHECK(desc->GetSize() == 3u);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 2u);
  CHECK(fields[0].GetBitWidth() == 16u);
  CHECK(fields[0].GetBitShift() == 0u);
  CHECK(fields[0].GetBitMask() == 0xffffu);
  CHECK(fields[0].GetOffset() == 0u);
  CHECK(fields[0].GetSize() == 2u);
  CHECK(fields[1].GetBitWidth() == 1u);
  CHECK(fields[1].GetBitMask() == 0x1u);
  CHECK(fields[1].GetBitShift() == 0u);
  CHECK(fields[1].GetOffset() == 2u);
  CHECK(fields[1].GetSize() == 1u);
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldBoolWrongSize",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "bool a:2", &err);
  REQUIRE_FALSE(desc);
  REQUIRE(err == "field a: bit width must be 1 for bool type");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest BitfieldTooBig",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int16 a:17", &err);
  REQUIRE_FALSE(desc);
  REQUIRE(err == "field a: bit width 17 exceeds type size");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest DuplicateFieldName",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "int16 a; int8 a", &err);
  REQUIRE_FALSE(desc);
  REQUIRE(err == "duplicate field a");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest StringAllZeros",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[32]", &err);
  uint8_t data[32];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK(dynamic.GetStringField(field) == "");
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest StringRoundTrip",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[32]", &err);
  uint8_t data[32];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK(dynamic.SetStringField(field, "abc"));
  CHECK(dynamic.GetStringField(field) == "abc");
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTripEmbeddedNull",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[32]", &err);
  uint8_t data[32];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  std::string check{"ab\0c", 4};
  REQUIRE(check.size() == 4u);
  CHECK(dynamic.SetStringField(field, check));
  auto get = dynamic.GetStringField(field);
  CHECK(get == check);
  CHECK(4u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest, "DynamicStructTest StringRoundTripTooLong",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK_FALSE(dynamic.SetStringField(field, "abc"));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "ab");
  CHECK(2u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTripPartial2ByteUtf8",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK_FALSE(dynamic.SetStringField(field, "a\u0234"));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a");
  CHECK(1u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip2ByteUtf8",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[3]", &err);
  uint8_t data[3];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK(dynamic.SetStringField(field, "a\u0234"));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a\u0234");
  CHECK(3u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip3ByteUtf8",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[4]", &err);
  uint8_t data[4];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK(dynamic.SetStringField(field, "a\u1234"));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a\u1234");
  CHECK(4u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip3ByteUtf8PartialFirstByte",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK_FALSE(dynamic.SetStringField(field, "a\u1234"));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a");
  CHECK(1u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip3ByteUtf8PartialSecondByte",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[3]", &err);
  uint8_t data[3];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK_FALSE(dynamic.SetStringField(field, "a\u1234"));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a");
  CHECK(1u == get.size());
}

// MSVC and GCC do surrogate pairs differently.
// Manually construct the 4 byte string
static constexpr char buffer[] = {
    static_cast<char>(0x61), static_cast<char>(0xf0), static_cast<char>(0x9f),
    static_cast<char>(0x90), static_cast<char>(0x80), static_cast<char>(0x00)};
static constexpr std::string_view fourByteUtf8String{buffer};

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip4ByteUtf8",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[5]", &err);
  uint8_t data[5];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  CHECK(get == fourByteUtf8String);
  CHECK(5u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip4ByteUtf8PartialFirstByte",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK_FALSE(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a");
  CHECK(1u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip4ByteUtf8PartialSecondByte",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[3]", &err);
  uint8_t data[3];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK_FALSE(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a");
  CHECK(1u == get.size());
}

TEST_CASE_METHOD(DynamicStructTest,
                 "DynamicStructTest StringRoundTrip4ByteUtf8PartialThirdByte",
                 "[wpiutil][struct]") {
  auto desc = db.Add("test", "char a[4]", &err);
  uint8_t data[4];
  std::memset(data, 0, sizeof(data));
  REQUIRE(desc->GetSize() == sizeof(data) / sizeof(data[0]));
  wpi::util::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  CHECK_FALSE(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  CHECK(get == "a");
  CHECK(1u == get.size());
}

struct SimpleTestParam {
  const char* schema;
  size_t size;
  StructFieldType type;
  bool isInt;
  bool isUint;
  unsigned int bitWidth;
  uint64_t bitMask;
  uint64_t minVal;
  uint64_t maxVal;
};

std::ostream& operator<<(std::ostream& os, const SimpleTestParam& param) {
  return os << "SimpleTestParam(Schema: \"" << param.schema << "\")";
}

static void CheckSimpleStruct(const SimpleTestParam& param) {
  StructDescriptorDatabase db;
  std::string err;
  auto desc = db.Add("test", param.schema, &err);
  REQUIRE(desc);
  REQUIRE(desc->GetName() == "test");
  REQUIRE(desc->GetSchema() == param.schema);
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 1u);
  CHECK(fields[0].GetParent() == desc);
  CHECK(fields[0].GetName() == "a");
  CHECK(fields[0].IsInt() == param.isInt);
  CHECK(fields[0].IsUint() == param.isUint);
  CHECK_FALSE(fields[0].IsArray());
  if (param.type != StructFieldType::STRUCT) {
    REQUIRE(desc->IsValid());
    REQUIRE(desc->GetSize() == param.size);
    REQUIRE(fields[0].GetSize() == param.size);
    REQUIRE(fields[0].GetBitWidth() == param.bitWidth);
    REQUIRE(fields[0].GetBitMask() == param.bitMask);
  } else {
    REQUIRE_FALSE(desc->IsValid());
    REQUIRE(fields[0].GetStruct());
  }
}

static void CheckSimpleStructArray(const SimpleTestParam& param) {
  StructDescriptorDatabase db;
  std::string err;
  auto desc = db.Add("test", param.schema + std::string{"[2]"}, &err);
  REQUIRE(desc);
  REQUIRE(desc->GetName() == "test");
  REQUIRE(desc->GetSchema() == (param.schema + std::string{"[2]"}));
  auto& fields = desc->GetFields();
  REQUIRE(fields.size() == 1u);
  CHECK(fields[0].GetParent() == desc);
  CHECK(fields[0].GetName() == "a");
  CHECK(fields[0].IsInt() == param.isInt);
  CHECK(fields[0].IsUint() == param.isUint);
  CHECK(fields[0].IsArray());
  CHECK(fields[0].GetArraySize() == 2u);
  if (param.type != StructFieldType::STRUCT) {
    REQUIRE(desc->IsValid());
    REQUIRE(desc->GetSize() == param.size * 2u);
  } else {
    REQUIRE_FALSE(desc->IsValid());
    REQUIRE(fields[0].GetStruct());
  }
}

static int64_t SignExtend(uint64_t value, size_t size) {
  switch (size) {
    case 1:
      return static_cast<int8_t>(value);
    case 2:
      return static_cast<int16_t>(value);
    case 4:
      return static_cast<int32_t>(value);
    default:
      return value;
  }
}

static void CheckSimpleStructIntRoundTrip(const SimpleTestParam& param) {
  if (param.type == StructFieldType::STRUCT) {
    return;
  }
  StructDescriptorDatabase db;
  std::string err;
  auto desc = db.Add("test", param.schema, &err);
  REQUIRE(desc);
  REQUIRE(desc->IsValid());
  std::vector<uint8_t> dest(desc->GetSize());
  auto field = desc->FindFieldByName("a");
  REQUIRE(field);
  wpi::util::MutableDynamicStruct dynamic(desc, dest);
  if (param.isInt) {
    {
      int64_t value = SignExtend(param.minVal, field->GetSize());
      dynamic.SetIntField(field, value);
      CHECK(dynamic.GetIntField(field) == value);
    }
    {
      int64_t value = SignExtend(param.maxVal, field->GetSize());
      dynamic.SetIntField(field, value);
      CHECK(dynamic.GetIntField(field) == value);
    }
  } else if (param.isUint) {
    {
      uint64_t value = param.minVal;
      dynamic.SetUintField(field, value);
      CHECK(dynamic.GetUintField(field) == value);
    }
    {
      uint64_t value = param.maxVal;
      dynamic.SetUintField(field, value);
      CHECK(dynamic.GetUintField(field) == value);
    }
  } else if (param.type == StructFieldType::BOOL) {
    dynamic.SetBoolField(field, false);
    CHECK_FALSE(dynamic.GetBoolField(field));
    dynamic.SetBoolField(field, true);
    CHECK(dynamic.GetBoolField(field));
  }
}

static SimpleTestParam simpleTests[] = {
    {"bool a", 1, StructFieldType::BOOL, false, false, 8, UINT8_MAX, 0, 0},
    {"char a", 1, StructFieldType::CHAR, false, false, 8, UINT8_MAX, 0, 0},
    {"int8 a", 1, StructFieldType::INT8, true, false, 8, UINT8_MAX,
     static_cast<uint64_t>(std::numeric_limits<int8_t>::min()),
     std::numeric_limits<int8_t>::max()},
    {"int16 a", 2, StructFieldType::INT16, true, false, 16, UINT16_MAX,
     static_cast<uint64_t>(std::numeric_limits<int16_t>::min()),
     std::numeric_limits<int16_t>::max()},
    {"int32 a", 4, StructFieldType::INT32, true, false, 32, UINT32_MAX,
     static_cast<uint64_t>(std::numeric_limits<int32_t>::min()),
     std::numeric_limits<int32_t>::max()},
    {"int64 a", 8, StructFieldType::INT64, true, false, 64, UINT64_MAX,
     static_cast<uint64_t>(std::numeric_limits<int64_t>::min()),
     std::numeric_limits<int64_t>::max()},
    {"uint8 a", 1, StructFieldType::UINT8, false, true, 8, UINT8_MAX,
     std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()},
    {"uint16 a", 2, StructFieldType::UINT16, false, true, 16, UINT16_MAX,
     std::numeric_limits<uint16_t>::min(),
     std::numeric_limits<uint16_t>::max()},
    {"uint32 a", 4, StructFieldType::UINT32, false, true, 32, UINT32_MAX,
     std::numeric_limits<uint32_t>::min(),
     std::numeric_limits<uint32_t>::max()},
    {"uint64 a", 8, StructFieldType::UINT64, false, true, 64, UINT64_MAX,
     std::numeric_limits<uint64_t>::min(),
     std::numeric_limits<uint64_t>::max()},
    {"float a", 4, StructFieldType::FLOAT, false, false, 32, UINT32_MAX, 0, 0},
    {"float32 a", 4, StructFieldType::FLOAT, false, false, 32, UINT32_MAX, 0,
     0},
    {"double a", 8, StructFieldType::DOUBLE, false, false, 64, UINT64_MAX, 0,
     0},
    {"float64 a", 8, StructFieldType::DOUBLE, false, false, 64, UINT64_MAX, 0,
     0},
    {"foo a", 0, StructFieldType::STRUCT, false, false, 0, 0, 0, 0},
};

TEST_CASE("DynamicSimpleStructTest Check", "[wpiutil][struct]") {
  const auto& param = GENERATE_REF(Catch::Generators::from_range(simpleTests));
  CheckSimpleStruct(param);
}

TEST_CASE("DynamicSimpleStructTest Array", "[wpiutil][struct]") {
  const auto& param = GENERATE_REF(Catch::Generators::from_range(simpleTests));
  CheckSimpleStructArray(param);
}

TEST_CASE("DynamicSimpleStructTest IntRoundTrip", "[wpiutil][struct]") {
  const auto& param = GENERATE_REF(Catch::Generators::from_range(simpleTests));
  CheckSimpleStructIntRoundTrip(param);
}
