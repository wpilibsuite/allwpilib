// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/struct/DynamicStruct.h"  // NOLINT(build/include_order)

#include <stdint.h>

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
  auto desc2 = db.Add("test2", "foo a[2]", &err);
  ASSERT_TRUE(desc2);
  ASSERT_FALSE(desc2->IsValid());
  auto desc3 = db.Add("foo", "int32 a", &err);
  ASSERT_TRUE(desc3);
  ASSERT_TRUE(desc3->IsValid());
  ASSERT_TRUE(desc->IsValid());
  ASSERT_EQ(desc->GetSize(), 4u);
  ASSERT_TRUE(desc2->IsValid());
  ASSERT_EQ(desc2->GetSize(), 8u);
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

struct SimpleTestParam {
  const char* schema;
  size_t size;
  StructFieldType type;
  bool isInt;
  bool isUint;
  unsigned int bitWidth;
  uint64_t bitMask;
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

static SimpleTestParam simpleTests[] = {
    {"bool a", 1, StructFieldType::kBool, false, false, 8, UINT8_MAX},
    {"char a", 1, StructFieldType::kChar, false, false, 8, UINT8_MAX},
    {"int8 a", 1, StructFieldType::kInt8, true, false, 8, UINT8_MAX},
    {"int16 a", 2, StructFieldType::kInt16, true, false, 16, UINT16_MAX},
    {"int32 a", 4, StructFieldType::kInt32, true, false, 32, UINT32_MAX},
    {"int64 a", 8, StructFieldType::kInt64, true, false, 64, UINT64_MAX},
    {"uint8 a", 1, StructFieldType::kUint8, false, true, 8, UINT8_MAX},
    {"uint16 a", 2, StructFieldType::kUint16, false, true, 16, UINT16_MAX},
    {"uint32 a", 4, StructFieldType::kUint32, false, true, 32, UINT32_MAX},
    {"uint64 a", 8, StructFieldType::kUint64, false, true, 64, UINT64_MAX},
    {"float a", 4, StructFieldType::kFloat, false, false, 32, UINT32_MAX},
    {"float32 a", 4, StructFieldType::kFloat, false, false, 32, UINT32_MAX},
    {"double a", 8, StructFieldType::kDouble, false, false, 64, UINT64_MAX},
    {"float64 a", 8, StructFieldType::kDouble, false, false, 64, UINT64_MAX},
    {"foo a", 0, StructFieldType::kStruct, false, false, 0, 0},
};

INSTANTIATE_TEST_SUITE_P(DynamicSimpleStructTests, DynamicSimpleStructTest,
                         ::testing::ValuesIn(simpleTests));
