// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/ct_string.hpp"

#include <string>
#include <string_view>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

TEST_CASE("CtStringTest Concat", "[wpiutil]") {
  using namespace wpi::util::literals;
  constexpr std::string_view astring = "name";
  constexpr int arrsize = 5;
  constexpr auto str = Concat(
      wpi::util::ct_string<char, std::char_traits<char>, astring.size()>{
          astring},
      "["_ct_string, wpi::util::NumToCtString<arrsize>(), "]"_ct_string);
  static_assert(str.size() == 7);
  REQUIRE(std::string{str} == "name[5]");
}

TEST_CASE("CtStringTest OperatorPlus", "[wpiutil]") {
  using namespace wpi::util::literals;
  constexpr std::string_view astring = "name";
  constexpr auto str =
      wpi::util::ct_string<char, std::char_traits<char>, astring.size()>{
          astring} +
      "[]"_ct_string;
  static_assert(str.size() == 6);
  REQUIRE(std::string{str} == "name[]");
}

TEST_CASE("CtStringTest StringViewConversion", "[wpiutil]") {
  using namespace wpi::util::literals;
  constexpr auto str = "[]"_ct_string;
  std::string_view sv = str;
  REQUIRE(sv == "[]");
}
