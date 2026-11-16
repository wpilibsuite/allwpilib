// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "wpi/util/SmallString.hpp"
#include "wpi/util/StringExtras.hpp"

using namespace wpi::util;

namespace {

TEST_CASE("UnescapeCStringTest Basic", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("abc\\\\\\a\\b\\f\\n\\r\\t\\v\\", buf);
  CHECK(out == "abc\\\a\b\f\n\r\t\v\\");
  CHECK(rem.empty());
}

TEST_CASE("UnescapeCStringTest QuoteEnd", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("abc\\\"\"123", buf);
  CHECK(out == "abc\"");
  CHECK(rem == "\"123");
}

TEST_CASE("UnescapeCStringTest Hex", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\xfe\\xFE\\x01", buf);
  CHECK(out == "\xfe\xFE\x01");
  CHECK(rem.empty());
}

TEST_CASE("UnescapeCStringTest HexPartial", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\xz\\x5z\\x2", buf);
  CHECK(out == "xz\x05z\x02");
  CHECK(rem.empty());
}

TEST_CASE("UnescapeCStringTest HexPartial2", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\x", buf);
  CHECK(out == "x");
  CHECK(rem.empty());
}

TEST_CASE("UnescapeCStringTest Octal", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("\\3\\11\\222\\4", buf);
  CHECK(out == "\3\11\222\4");
  CHECK(rem.empty());
}

TEST_CASE("UnescapeCStringTest EmptyString", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("", buf);
  CHECK(out == "");
}

TEST_CASE("UnescapeCStringTest ShortString", "[wpiutil]") {
  SmallString<64> buf;
  auto [out, rem] = UnescapeCString("a", buf);
  CHECK(out == "a");
}

TEST_CASE("UnescapeCStringTest NoEscapesString", "[wpiutil]") {
  SmallString<64> buf;
  std::string_view input = "abcdefghijklmnopqrstuvwxyz1234567890";
  auto [out, rem] = UnescapeCString(input, buf);
  CHECK(out == input);
}

}  // namespace
