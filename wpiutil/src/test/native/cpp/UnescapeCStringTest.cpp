// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/StringExtras.h"

using namespace wpi;

namespace {

TEST(UnescapeCStringTest, Basic) {
  auto [out, rem] = UnescapeCString("abc\\\\\\a\\b\\f\\n\\r\\t\\v\\");
  EXPECT_EQ(out, "abc\\\a\b\f\n\r\t\v\\");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, QuoteEnd) {
  auto [out, rem] = UnescapeCString("abc\\\"\"123");
  EXPECT_EQ(out, "abc\"");
  EXPECT_EQ(rem, "\"123");
}

TEST(UnescapeCStringTest, Hex) {
  auto [out, rem] = UnescapeCString("\\xfe\\xFE\\x01");
  EXPECT_EQ(out, "\xfe\xFE\x01");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, HexPartial) {
  auto [out, rem] = UnescapeCString("\\xz\\x5z\\x2");
  EXPECT_EQ(out, "xz\x05z\x02");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, HexPartial2) {
  auto [out, rem] = UnescapeCString("\\x");
  EXPECT_EQ(out, "x");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, Octal) {
  auto [out, rem] = UnescapeCString("\\3\\11\\222\\4");
  EXPECT_EQ(out, "\3\11\222\4");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, EmptyString) {
  auto [out, rem] = UnescapeCString("");
  EXPECT_EQ(out, "");
}

TEST(UnescapeCStringTest, ShortString) {
  auto [out, rem] = UnescapeCString("a");
  EXPECT_EQ(out, "a");
}

TEST(UnescapeCStringTest, NoEscapesString) {
  std::string_view input = "abcdefghijklmnopqrstuvwxyz1234567890";
  auto [out, rem] = UnescapeCString(input);
  EXPECT_EQ(out, input);
}

}  // namespace
