// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/struct/SchemaParser.hpp"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

using namespace wpi::structparser;

TEST(StructParserTest, Empty) {
  Parser p{""};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_TRUE(schema.declarations.empty());
}

TEST(StructParserTest, EmptySemicolon) {
  Parser p{";"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_TRUE(schema.declarations.empty());
}

TEST(StructParserTest, Simple) {
  Parser p{"int32 a"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  EXPECT_EQ(decl.arraySize, 1u);
}

TEST(StructParserTest, SimpleTrailingSemi) {
  Parser p{"int32 a;"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
}

TEST(StructParserTest, Array) {
  Parser p{"int32 a[2]"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  EXPECT_EQ(decl.arraySize, 2u);
}

TEST(StructParserTest, ArrayTrailingSemi) {
  Parser p{"int32 a[2];"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
}

TEST(StructParserTest, Bitfield) {
  Parser p{"int32 a:2"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  EXPECT_EQ(decl.bitWidth, 2u);
}

TEST(StructParserTest, BitfieldTrailingSemi) {
  Parser p{"int32 a:2;"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
}

TEST(StructParserTest, EnumKeyword) {
  Parser p{"enum {x=1} int32 a;"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  ASSERT_EQ(decl.enumValues.size(), 1u);
  EXPECT_EQ(decl.enumValues[0].first, "x");
  EXPECT_EQ(decl.enumValues[0].second, 1);
}

TEST(StructParserTest, EnumNoKeyword) {
  Parser p{"{x=1} int32 a;"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  ASSERT_EQ(decl.enumValues.size(), 1u);
  EXPECT_EQ(decl.enumValues[0].first, "x");
  EXPECT_EQ(decl.enumValues[0].second, 1);
}

TEST(StructParserTest, EnumNoValues) {
  Parser p{"{} int32 a;"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  ASSERT_TRUE(decl.enumValues.empty());
}

TEST(StructParserTest, EnumMultipleValues) {
  Parser p{"{x=1,y=-2} int32 a;"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  ASSERT_EQ(decl.enumValues.size(), 2u);
  EXPECT_EQ(decl.enumValues[0].first, "x");
  EXPECT_EQ(decl.enumValues[0].second, 1);
  EXPECT_EQ(decl.enumValues[1].first, "y");
  EXPECT_EQ(decl.enumValues[1].second, -2);
}

TEST(StructParserTest, EnumTrailingComma) {
  Parser p{"{x=1,y=2,} int32 a;"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 1u);
  auto& decl = schema.declarations[0];
  EXPECT_EQ(decl.typeString, "int32");
  EXPECT_EQ(decl.name, "a");
  ASSERT_EQ(decl.enumValues.size(), 2u);
  EXPECT_EQ(decl.enumValues[0].first, "x");
  EXPECT_EQ(decl.enumValues[0].second, 1);
  EXPECT_EQ(decl.enumValues[1].first, "y");
  EXPECT_EQ(decl.enumValues[1].second, 2);
}

TEST(StructParserTest, MultipleNoTrailingSemi) {
  Parser p{"int32 a; int16 b"};
  ParsedSchema schema;
  ASSERT_TRUE(p.Parse(&schema));
  ASSERT_EQ(schema.declarations.size(), 2u);
  EXPECT_EQ(schema.declarations[0].typeString, "int32");
  EXPECT_EQ(schema.declarations[0].name, "a");
  EXPECT_EQ(schema.declarations[1].typeString, "int16");
  EXPECT_EQ(schema.declarations[1].name, "b");
}

TEST(StructParserTest, ErrBitfieldArray) {
  Parser p{"int32 a[1]:2"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "10: expected ';', got ':'");
}

TEST(StructParserTest, ErrNoArrayValue) {
  Parser p{"int32 a[]"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "8: expected integer, got ']'");
}

TEST(StructParserTest, ErrNoBitfieldValue) {
  Parser p{"int32 a:"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "8: expected integer, got ''");
}

TEST(StructParserTest, ErrNoNameArray) {
  Parser p{"int32 [2]"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "6: expected identifier, got '['");
}

TEST(StructParserTest, ErrNoNameBitField) {
  Parser p{"int32 :2"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "6: expected identifier, got ':'");
}

TEST(StructParserTest, NegativeBitField) {
  Parser p{"int32 a:-1"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "8: bitfield width '-1' is not a positive integer");
}

TEST(StructParserTest, NegativeArraySize) {
  Parser p{"int32 a[-1]"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "8: array size '-1' is not a positive integer");
}

TEST(StructParserTest, ZeroBitField) {
  Parser p{"int32 a:0"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "8: bitfield width '0' is not a positive integer");
}

TEST(StructParserTest, ZeroArraySize) {
  Parser p{"int32 a[0]"};
  ParsedSchema schema;
  ASSERT_FALSE(p.Parse(&schema));
  ASSERT_EQ(p.GetError(), "8: array size '0' is not a positive integer");
}
