// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/struct/SchemaParser.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using namespace wpi::util::structparser;

TEST_CASE("StructParserTest Empty", "[wpiutil][struct]") {
  Parser p{""};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE(schema.declarations.empty());
}

TEST_CASE("StructParserTest EmptySemicolon", "[wpiutil][struct]") {
  Parser p{";"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE(schema.declarations.empty());
}

TEST_CASE("StructParserTest Simple", "[wpiutil][struct]") {
  Parser p{"int32 a"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  CHECK((decl.arraySize) == (1u));
}

TEST_CASE("StructParserTest SimpleTrailingSemi", "[wpiutil][struct]") {
  Parser p{"int32 a;"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
}

TEST_CASE("StructParserTest Array", "[wpiutil][struct]") {
  Parser p{"int32 a[2]"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  CHECK((decl.arraySize) == (2u));
}

TEST_CASE("StructParserTest ArrayTrailingSemi", "[wpiutil][struct]") {
  Parser p{"int32 a[2];"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
}

TEST_CASE("StructParserTest Bitfield", "[wpiutil][struct]") {
  Parser p{"int32 a:2"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  CHECK((decl.bitWidth) == (2u));
}

TEST_CASE("StructParserTest BitfieldTrailingSemi", "[wpiutil][struct]") {
  Parser p{"int32 a:2;"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
}

TEST_CASE("StructParserTest EnumKeyword", "[wpiutil][struct]") {
  Parser p{"enum {x=1} int32 a;"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  REQUIRE((decl.enumValues.size()) == (1u));
  CHECK((decl.enumValues[0].first) == ("x"));
  CHECK((decl.enumValues[0].second) == (1));
}

TEST_CASE("StructParserTest EnumNoKeyword", "[wpiutil][struct]") {
  Parser p{"{x=1} int32 a;"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  REQUIRE((decl.enumValues.size()) == (1u));
  CHECK((decl.enumValues[0].first) == ("x"));
  CHECK((decl.enumValues[0].second) == (1));
}

TEST_CASE("StructParserTest EnumNoValues", "[wpiutil][struct]") {
  Parser p{"{} int32 a;"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  REQUIRE(decl.enumValues.empty());
}

TEST_CASE("StructParserTest EnumMultipleValues", "[wpiutil][struct]") {
  Parser p{"{x=1,y=-2} int32 a;"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  REQUIRE((decl.enumValues.size()) == (2u));
  CHECK((decl.enumValues[0].first) == ("x"));
  CHECK((decl.enumValues[0].second) == (1));
  CHECK((decl.enumValues[1].first) == ("y"));
  CHECK((decl.enumValues[1].second) == (-2));
}

TEST_CASE("StructParserTest EnumTrailingComma", "[wpiutil][struct]") {
  Parser p{"{x=1,y=2,} int32 a;"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (1u));
  auto& decl = schema.declarations[0];
  CHECK((decl.typeString) == ("int32"));
  CHECK((decl.name) == ("a"));
  REQUIRE((decl.enumValues.size()) == (2u));
  CHECK((decl.enumValues[0].first) == ("x"));
  CHECK((decl.enumValues[0].second) == (1));
  CHECK((decl.enumValues[1].first) == ("y"));
  CHECK((decl.enumValues[1].second) == (2));
}

TEST_CASE("StructParserTest MultipleNoTrailingSemi", "[wpiutil][struct]") {
  Parser p{"int32 a; int16 b"};
  ParsedSchema schema;
  REQUIRE(p.Parse(&schema));
  REQUIRE((schema.declarations.size()) == (2u));
  CHECK((schema.declarations[0].typeString) == ("int32"));
  CHECK((schema.declarations[0].name) == ("a"));
  CHECK((schema.declarations[1].typeString) == ("int16"));
  CHECK((schema.declarations[1].name) == ("b"));
}

TEST_CASE("StructParserTest ErrBitfieldArray", "[wpiutil][struct]") {
  Parser p{"int32 a[1]:2"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) == ("10: expected ';', got ':'"));
}

TEST_CASE("StructParserTest ErrNoArrayValue", "[wpiutil][struct]") {
  Parser p{"int32 a[]"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) == ("8: expected integer, got ']'"));
}

TEST_CASE("StructParserTest ErrNoBitfieldValue", "[wpiutil][struct]") {
  Parser p{"int32 a:"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) == ("8: expected integer, got ''"));
}

TEST_CASE("StructParserTest ErrNoNameArray", "[wpiutil][struct]") {
  Parser p{"int32 [2]"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) == ("6: expected identifier, got '['"));
}

TEST_CASE("StructParserTest ErrNoNameBitField", "[wpiutil][struct]") {
  Parser p{"int32 :2"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) == ("6: expected identifier, got ':'"));
}

TEST_CASE("StructParserTest NegativeBitField", "[wpiutil][struct]") {
  Parser p{"int32 a:-1"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) ==
          ("8: bitfield width '-1' is not a positive integer"));
}

TEST_CASE("StructParserTest NegativeArraySize", "[wpiutil][struct]") {
  Parser p{"int32 a[-1]"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) == ("8: array size '-1' is not a positive integer"));
}

TEST_CASE("StructParserTest ZeroBitField", "[wpiutil][struct]") {
  Parser p{"int32 a:0"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) ==
          ("8: bitfield width '0' is not a positive integer"));
}

TEST_CASE("StructParserTest ZeroArraySize", "[wpiutil][struct]") {
  Parser p{"int32 a[0]"};
  ParsedSchema schema;
  REQUIRE_FALSE(p.Parse(&schema));
  REQUIRE((p.GetError()) == ("8: array size '0' is not a positive integer"));
}
