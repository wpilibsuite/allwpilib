// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/struct/DynamicStruct.h"  // NOLINT(build/include_order)

#include <stdint.h>

#include <cstring>
#include <limits>
#include <string>
#include <vector>

#include <gtest/gtest.h>

using namespace wpi;

class DynamicStructTest : public ::testing::Test {
 protected:
  StructDescriptorDatabase db;
  std::string err;
};

TEST_F(DynamicStructTest, Empty) {
  auto desc = db.Add("test", "", &err);
  ASSERT_TRUE(desc);
  ASSERT_EQ(desc->GetName(), "test");
  ASSERT_EQ(desc->GetSchema(), "");
  ASSERT_TRUE(desc->GetFields().empty());
  ASSERT_TRUE(desc->IsValid());
  ASSERT_EQ(desc->GetSize(), 0u);
}

TEST_F(DynamicStructTest, NestedStruct) {
  auto desc = db.Add("test", "int32 a", &err);
  ASSERT_TRUE(desc);
  ASSERT_TRUE(desc->IsValid());
  auto desc2 = db.Add("test2", "test a", &err);
  ASSERT_TRUE(desc2);
  ASSERT_TRUE(desc2->IsValid());
  ASSERT_EQ(desc2->GetSize(), 4u);
}

TEST_F(DynamicStructTest, DelayedValid) {
  auto desc = db.Add("test", "foo a", &err);
  ASSERT_TRUE(desc);
  ASSERT_FALSE(desc->IsValid());
  auto desc2 = db.Add("test2", "foo a;foo b;", &err);
  ASSERT_TRUE(desc2);
  ASSERT_FALSE(desc2->IsValid());
  auto desc3 = db.Add("test3", "foo a[2]", &err);
  ASSERT_TRUE(desc3);
  ASSERT_FALSE(desc3->IsValid());
  auto desc4 = db.Add("foo", "int32 a", &err);
  ASSERT_TRUE(desc4);
  ASSERT_TRUE(desc4->IsValid());
  ASSERT_TRUE(desc->IsValid());
  ASSERT_EQ(desc->GetSize(), 4u);
  ASSERT_TRUE(desc2->IsValid());
  ASSERT_EQ(desc2->GetSize(), 8u);
  ASSERT_TRUE(desc3->IsValid());
  ASSERT_EQ(desc3->GetSize(), 8u);
}

TEST_F(DynamicStructTest, ReuseNestedStructDelayed) {
  auto desc2 = db.Add("test2", "test a;test b;", &err);
  auto desc = db.Add("test", "int32 a; uint16 b; int16 c;", &err);
  ASSERT_TRUE(desc);
  ASSERT_TRUE(desc->IsValid());
  ASSERT_TRUE(desc2);
  ASSERT_TRUE(desc2->IsValid());
  ASSERT_EQ(desc2->GetSize(), 16u);
  auto fields = desc2->GetFields();
  ASSERT_EQ(fields[0].GetOffset(), 0u);
  ASSERT_EQ(fields[0].GetName(), "a");
  ASSERT_EQ(fields[1].GetOffset(), 8u);
  ASSERT_EQ(fields[1].GetName(), "b");
}

TEST_F(DynamicStructTest, InvalidBitfield) {
  auto desc = db.Add("test", "float a:1", &err);
  EXPECT_FALSE(desc);
  EXPECT_EQ(err, "field a: type float cannot be bitfield");

  desc = db.Add("test", "double a:1", &err);
  EXPECT_FALSE(desc);
  EXPECT_EQ(err, "field a: type double cannot be bitfield");

  desc = db.Add("test", "foo a:1", &err);
  EXPECT_FALSE(desc);
  EXPECT_EQ(err, "field a: type foo cannot be bitfield");
}

TEST_F(DynamicStructTest, CircularStructReference) {
  auto desc = db.Add("test", "test a", &err);
  ASSERT_FALSE(desc);
  ASSERT_EQ(err, "field a: recursive struct reference");
}

TEST_F(DynamicStructTest, NestedCircularStructRef) {
  auto desc = db.Add("test", "foo a", &err);
  ASSERT_TRUE(desc);
  auto desc2 = db.Add("foo", "bar a", &err);
  ASSERT_TRUE(desc2);
  auto desc3 = db.Add("bar", "test a", &err);
  ASSERT_FALSE(desc3);
  ASSERT_EQ(err, "circular struct reference: bar <- foo <- test");

  // ok
  auto desc4 = db.Add("baz", "bar a", &err);
  ASSERT_TRUE(desc4);
  ASSERT_FALSE(desc4->IsValid());
}

TEST_F(DynamicStructTest, NestedCircularStructRef2) {
  auto desc = db.Add("test", "foo a", &err);
  ASSERT_TRUE(desc);
  auto desc2 = db.Add("bar", "test a", &err);
  ASSERT_TRUE(desc2);
  auto desc3 = db.Add("foo", "bar a", &err);
  ASSERT_FALSE(desc3);
  ASSERT_EQ(err, "circular struct reference: foo <- test <- bar");
}

TEST_F(DynamicStructTest, BitfieldBasic) {
  auto desc = db.Add("test", "int32 a:2; uint32 b:30", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 4u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].GetBitWidth(), 2u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0x3u);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 4u);
  EXPECT_EQ(fields[1].GetBitWidth(), 30u);
  EXPECT_EQ(fields[1].GetBitShift(), 2u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x3fffffffu);
  EXPECT_EQ(fields[1].GetOffset(), 0u);
  EXPECT_EQ(fields[1].GetSize(), 4u);
}

TEST_F(DynamicStructTest, BitfieldDiffType) {
  auto desc = db.Add("test", "int32 a:2; int16 b:2", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 6u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].GetBitWidth(), 2u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0x3u);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 4u);
  EXPECT_EQ(fields[1].GetBitWidth(), 2u);
  EXPECT_EQ(fields[1].GetBitShift(), 0u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x3u);
  EXPECT_EQ(fields[1].GetOffset(), 4u);
  EXPECT_EQ(fields[1].GetSize(), 2u);
}

TEST_F(DynamicStructTest, BitfieldOverflow) {
  auto desc = db.Add("test", "int8 a:4; int8 b:5", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 2u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].GetBitWidth(), 4u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0xfu);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 1u);
  EXPECT_EQ(fields[1].GetBitWidth(), 5u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x1fu);
  EXPECT_EQ(fields[1].GetBitShift(), 0u);
  EXPECT_EQ(fields[1].GetOffset(), 1u);
  EXPECT_EQ(fields[1].GetSize(), 1u);
}

TEST_F(DynamicStructTest, BitfieldBoolBegin8) {
  auto desc = db.Add("test", "bool a:1; int8 b:5", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 1u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].GetBitWidth(), 1u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0x1u);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 1u);
  EXPECT_EQ(fields[1].GetBitWidth(), 5u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x1fu);
  EXPECT_EQ(fields[1].GetBitShift(), 1u);
  EXPECT_EQ(fields[1].GetOffset(), 0u);
  EXPECT_EQ(fields[1].GetSize(), 1u);
}

TEST_F(DynamicStructTest, BitfieldBoolBegin16) {
  auto desc = db.Add("test", "bool a:1; int16 b:5", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 3u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].GetBitWidth(), 1u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0x1u);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 1u);
  EXPECT_EQ(fields[1].GetBitWidth(), 5u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x1fu);
  EXPECT_EQ(fields[1].GetBitShift(), 0u);
  EXPECT_EQ(fields[1].GetOffset(), 1u);
  EXPECT_EQ(fields[1].GetSize(), 2u);
}

TEST_F(DynamicStructTest, BitfieldBoolMid) {
  auto desc = db.Add("test", "int16 a:2; bool b:1; bool c:1; uint16 d:5", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 2u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 4u);
  EXPECT_EQ(fields[0].GetBitWidth(), 2u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0x3u);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 2u);
  EXPECT_EQ(fields[1].GetBitWidth(), 1u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x1u);
  EXPECT_EQ(fields[1].GetBitShift(), 2u);
  EXPECT_EQ(fields[1].GetOffset(), 0u);
  EXPECT_EQ(fields[1].GetSize(), 2u);
  EXPECT_EQ(fields[2].GetBitWidth(), 1u);
  EXPECT_EQ(fields[2].GetBitMask(), 0x1u);
  EXPECT_EQ(fields[2].GetBitShift(), 3u);
  EXPECT_EQ(fields[2].GetOffset(), 0u);
  EXPECT_EQ(fields[2].GetSize(), 2u);
  EXPECT_EQ(fields[3].GetBitWidth(), 5u);
  EXPECT_EQ(fields[3].GetBitMask(), 0x1fu);
  EXPECT_EQ(fields[3].GetBitShift(), 4u);
  EXPECT_EQ(fields[3].GetOffset(), 0u);
  EXPECT_EQ(fields[3].GetSize(), 2u);
}

TEST_F(DynamicStructTest, BitfieldBoolEnd) {
  auto desc = db.Add("test", "int16 a:15; bool b:1", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 2u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].GetBitWidth(), 15u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0x7fffu);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 2u);
  EXPECT_EQ(fields[1].GetBitWidth(), 1u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x1u);
  EXPECT_EQ(fields[1].GetBitShift(), 15u);
  EXPECT_EQ(fields[1].GetOffset(), 0u);
  EXPECT_EQ(fields[1].GetSize(), 2u);
}

TEST_F(DynamicStructTest, BitfieldBoolEnd2) {
  auto desc = db.Add("test", "int16 a:16; bool b:1", &err);
  ASSERT_TRUE(desc);
  EXPECT_EQ(desc->GetSize(), 3u);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].GetBitWidth(), 16u);
  EXPECT_EQ(fields[0].GetBitShift(), 0u);
  EXPECT_EQ(fields[0].GetBitMask(), 0xffffu);
  EXPECT_EQ(fields[0].GetOffset(), 0u);
  EXPECT_EQ(fields[0].GetSize(), 2u);
  EXPECT_EQ(fields[1].GetBitWidth(), 1u);
  EXPECT_EQ(fields[1].GetBitMask(), 0x1u);
  EXPECT_EQ(fields[1].GetBitShift(), 0u);
  EXPECT_EQ(fields[1].GetOffset(), 2u);
  EXPECT_EQ(fields[1].GetSize(), 1u);
}

TEST_F(DynamicStructTest, BitfieldBoolWrongSize) {
  auto desc = db.Add("test", "bool a:2", &err);
  ASSERT_FALSE(desc);
  ASSERT_EQ(err, "field a: bit width must be 1 for bool type");
}

TEST_F(DynamicStructTest, BitfieldTooBig) {
  auto desc = db.Add("test", "int16 a:17", &err);
  ASSERT_FALSE(desc);
  ASSERT_EQ(err, "field a: bit width 17 exceeds type size");
}

TEST_F(DynamicStructTest, DuplicateFieldName) {
  auto desc = db.Add("test", "int16 a; int8 a", &err);
  ASSERT_FALSE(desc);
  ASSERT_EQ(err, "duplicate field a");
}

TEST_F(DynamicStructTest, StringAllZeros) {
  auto desc = db.Add("test", "char a[32]", &err);
  uint8_t data[32];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_EQ(dynamic.GetStringField(field), "");
}

TEST_F(DynamicStructTest, StringRoundTrip) {
  auto desc = db.Add("test", "char a[32]", &err);
  uint8_t data[32];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_TRUE(dynamic.SetStringField(field, "abc"));
  EXPECT_EQ(dynamic.GetStringField(field), "abc");
}

TEST_F(DynamicStructTest, StringRoundTripEmbeddedNull) {
  auto desc = db.Add("test", "char a[32]", &err);
  uint8_t data[32];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  std::string check{"ab\0c", 4};
  ASSERT_EQ(check.size(), 4u);
  EXPECT_TRUE(dynamic.SetStringField(field, check));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, check);
  EXPECT_EQ(4u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTripTooLong) {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_FALSE(dynamic.SetStringField(field, "abc"));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "ab");
  EXPECT_EQ(2u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTripPartial2ByteUtf8) {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_FALSE(dynamic.SetStringField(field, "a\u0234"));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a");
  EXPECT_EQ(1u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTrip2ByteUtf8) {
  auto desc = db.Add("test", "char a[3]", &err);
  uint8_t data[3];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_TRUE(dynamic.SetStringField(field, "a\u0234"));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a\u0234");
  EXPECT_EQ(3u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTrip3ByteUtf8) {
  auto desc = db.Add("test", "char a[4]", &err);
  uint8_t data[4];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_TRUE(dynamic.SetStringField(field, "a\u1234"));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a\u1234");
  EXPECT_EQ(4u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTrip3ByteUtf8PartialFirstByte) {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_FALSE(dynamic.SetStringField(field, "a\u1234"));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a");
  EXPECT_EQ(1u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTrip3ByteUtf8PartialSecondByte) {
  auto desc = db.Add("test", "char a[3]", &err);
  uint8_t data[3];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_FALSE(dynamic.SetStringField(field, "a\u1234"));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a");
  EXPECT_EQ(1u, get.size());
}

// MSVC and GCC do surrogate pairs differently.
// Manually construct the 4 byte string
static constexpr char buffer[] = {
    static_cast<char>(0x61), static_cast<char>(0xf0), static_cast<char>(0x9f),
    static_cast<char>(0x90), static_cast<char>(0x80), static_cast<char>(0x00)};
static constexpr std::string_view fourByteUtf8String{buffer};

TEST_F(DynamicStructTest, StringRoundTrip4ByteUtf8) {
  auto desc = db.Add("test", "char a[5]", &err);
  uint8_t data[5];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_TRUE(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, fourByteUtf8String);
  EXPECT_EQ(5u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTrip4ByteUtf8PartialFirstByte) {
  auto desc = db.Add("test", "char a[2]", &err);
  uint8_t data[2];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_FALSE(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a");
  EXPECT_EQ(1u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTrip4ByteUtf8PartialSecondByte) {
  auto desc = db.Add("test", "char a[3]", &err);
  uint8_t data[3];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_FALSE(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a");
  EXPECT_EQ(1u, get.size());
}

TEST_F(DynamicStructTest, StringRoundTrip4ByteUtf8PartialThirdByte) {
  auto desc = db.Add("test", "char a[4]", &err);
  uint8_t data[4];
  std::memset(data, 0, sizeof(data));
  ASSERT_EQ(desc->GetSize(), sizeof(data) / sizeof(data[0]));
  wpi::MutableDynamicStruct dynamic{desc, data};
  auto field = desc->FindFieldByName("a");
  EXPECT_FALSE(dynamic.SetStringField(field, fourByteUtf8String));
  auto get = dynamic.GetStringField(field);
  EXPECT_EQ(get, "a");
  EXPECT_EQ(1u, get.size());
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

class DynamicSimpleStructTest
    : public ::testing::TestWithParam<SimpleTestParam> {
 protected:
  StructDescriptorDatabase db;
  std::string err;
};

TEST_P(DynamicSimpleStructTest, Check) {
  auto desc = db.Add("test", GetParam().schema, &err);
  ASSERT_TRUE(desc);
  ASSERT_EQ(desc->GetName(), "test");
  ASSERT_EQ(desc->GetSchema(), GetParam().schema);
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 1u);
  EXPECT_EQ(fields[0].GetParent(), desc);
  EXPECT_EQ(fields[0].GetName(), "a");
  EXPECT_EQ(fields[0].IsInt(), GetParam().isInt);
  EXPECT_EQ(fields[0].IsUint(), GetParam().isUint);
  EXPECT_FALSE(fields[0].IsArray());
  if (GetParam().type != StructFieldType::kStruct) {
    ASSERT_TRUE(desc->IsValid());
    ASSERT_EQ(desc->GetSize(), GetParam().size);
    ASSERT_EQ(fields[0].GetSize(), GetParam().size);
    ASSERT_EQ(fields[0].GetBitWidth(), GetParam().bitWidth);
    ASSERT_EQ(fields[0].GetBitMask(), GetParam().bitMask);
  } else {
    ASSERT_FALSE(desc->IsValid());
    ASSERT_TRUE(fields[0].GetStruct());
  }
}

TEST_P(DynamicSimpleStructTest, Array) {
  auto desc = db.Add("test", GetParam().schema + std::string{"[2]"}, &err);
  ASSERT_TRUE(desc);
  ASSERT_EQ(desc->GetName(), "test");
  ASSERT_EQ(desc->GetSchema(), GetParam().schema + std::string{"[2]"});
  auto& fields = desc->GetFields();
  ASSERT_EQ(fields.size(), 1u);
  EXPECT_EQ(fields[0].GetParent(), desc);
  EXPECT_EQ(fields[0].GetName(), "a");
  EXPECT_EQ(fields[0].IsInt(), GetParam().isInt);
  EXPECT_EQ(fields[0].IsUint(), GetParam().isUint);
  EXPECT_TRUE(fields[0].IsArray());
  EXPECT_EQ(fields[0].GetArraySize(), 2u);
  if (GetParam().type != StructFieldType::kStruct) {
    ASSERT_TRUE(desc->IsValid());
    ASSERT_EQ(desc->GetSize(), GetParam().size * 2u);
  } else {
    ASSERT_FALSE(desc->IsValid());
    ASSERT_TRUE(fields[0].GetStruct());
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

TEST_P(DynamicSimpleStructTest, IntRoundTrip) {
  if (GetParam().type == StructFieldType::kStruct) {
    return;
  }
  auto desc = db.Add("test", GetParam().schema, &err);
  ASSERT_TRUE(desc);
  ASSERT_TRUE(desc->IsValid());
  std::vector<uint8_t> dest(desc->GetSize());
  auto field = desc->FindFieldByName("a");
  ASSERT_TRUE(field);
  wpi::MutableDynamicStruct dynamic(desc, dest);
  if (GetParam().isInt) {
    {
      int64_t value = SignExtend(GetParam().minVal, field->GetSize());
      dynamic.SetIntField(field, value);
      EXPECT_EQ(dynamic.GetIntField(field), value);
    }
    {
      int64_t value = SignExtend(GetParam().maxVal, field->GetSize());
      dynamic.SetIntField(field, value);
      EXPECT_EQ(dynamic.GetIntField(field), value);
    }
  } else if (GetParam().isUint) {
    {
      uint64_t value = GetParam().minVal;
      dynamic.SetUintField(field, value);
      EXPECT_EQ(dynamic.GetUintField(field), value);
    }
    {
      uint64_t value = GetParam().maxVal;
      dynamic.SetUintField(field, value);
      EXPECT_EQ(dynamic.GetUintField(field), value);
    }
  } else if (GetParam().type == StructFieldType::kBool) {
    dynamic.SetBoolField(field, false);
    EXPECT_FALSE(dynamic.GetBoolField(field));
    dynamic.SetBoolField(field, true);
    EXPECT_TRUE(dynamic.GetBoolField(field));
  }
}

static SimpleTestParam simpleTests[] = {
    {"bool a", 1, StructFieldType::kBool, false, false, 8, UINT8_MAX, 0, 0},
    {"char a", 1, StructFieldType::kChar, false, false, 8, UINT8_MAX, 0, 0},
    {"int8 a", 1, StructFieldType::kInt8, true, false, 8, UINT8_MAX,
     static_cast<uint64_t>(std::numeric_limits<int8_t>::min()),
     std::numeric_limits<int8_t>::max()},
    {"int16 a", 2, StructFieldType::kInt16, true, false, 16, UINT16_MAX,
     static_cast<uint64_t>(std::numeric_limits<int16_t>::min()),
     std::numeric_limits<int16_t>::max()},
    {"int32 a", 4, StructFieldType::kInt32, true, false, 32, UINT32_MAX,
     static_cast<uint64_t>(std::numeric_limits<int32_t>::min()),
     std::numeric_limits<int32_t>::max()},
    {"int64 a", 8, StructFieldType::kInt64, true, false, 64, UINT64_MAX,
     static_cast<uint64_t>(std::numeric_limits<int64_t>::min()),
     std::numeric_limits<int64_t>::max()},
    {"uint8 a", 1, StructFieldType::kUint8, false, true, 8, UINT8_MAX,
     std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()},
    {"uint16 a", 2, StructFieldType::kUint16, false, true, 16, UINT16_MAX,
     std::numeric_limits<uint16_t>::min(),
     std::numeric_limits<uint16_t>::max()},
    {"uint32 a", 4, StructFieldType::kUint32, false, true, 32, UINT32_MAX,
     std::numeric_limits<uint32_t>::min(),
     std::numeric_limits<uint32_t>::max()},
    {"uint64 a", 8, StructFieldType::kUint64, false, true, 64, UINT64_MAX,
     std::numeric_limits<uint64_t>::min(),
     std::numeric_limits<uint64_t>::max()},
    {"float a", 4, StructFieldType::kFloat, false, false, 32, UINT32_MAX, 0, 0},
    {"float32 a", 4, StructFieldType::kFloat, false, false, 32, UINT32_MAX, 0,
     0},
    {"double a", 8, StructFieldType::kDouble, false, false, 64, UINT64_MAX, 0,
     0},
    {"float64 a", 8, StructFieldType::kDouble, false, false, 64, UINT64_MAX, 0,
     0},
    {"foo a", 0, StructFieldType::kStruct, false, false, 0, 0, 0, 0},
};

INSTANTIATE_TEST_SUITE_P(DynamicSimpleStructTests, DynamicSimpleStructTest,
                         ::testing::ValuesIn(simpleTests));
