// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/util/SmallString.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::util;

namespace {

TEST(UnescapeCStringTest, Basic) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("abc\\\\\\a\\b\\f\\n\\r\\t\\v\\", buf);
  EXPECT_EQ(out, "abc\\\a\b\f\n\r\t\v\\");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, QuoteEnd) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("abc\\\"\"123", buf);
  EXPECT_EQ(out, "abc\"");
  EXPECT_EQ(rem, "\"123");
}

TEST(UnescapeCStringTest, Hex) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\xfe\\xFE\\x01", buf);
  EXPECT_EQ(out, "\xfe\xFE\x01");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, HexPartial) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\xz\\x5z\\x2", buf);
  EXPECT_EQ(out, "xz\x05z\x02");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, HexPartial2) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\x", buf);
  EXPECT_EQ(out, "x");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, Octal) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\3\\11\\222\\4", buf);
  EXPECT_EQ(out, "\3\11\222\4");
  EXPECT_TRUE(rem.empty());
}

TEST(UnescapeCStringTest, EmptyString) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("", buf);
  EXPECT_EQ(out, "");
}

TEST(UnescapeCStringTest, ShortString) {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("a", buf);
  EXPECT_EQ(out, "a");
}

TEST(UnescapeCStringTest, NoEscapesString) {
  SmallString<64> buf;
  std::string_view input = "abcdefghijklmnopqrstuvwxyz1234567890";
  auto [out, rem] = UnescapeCString(input, buf);
  EXPECT_EQ(out, input);
}

}  // namespace
