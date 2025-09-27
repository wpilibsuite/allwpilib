// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/ct_string.hpp"  // NOLINT(build/include_order)

#include <string>
#include <string_view>

#include <gtest/gtest.h>

TEST(CtStringTest, Concat) {
  using namespace wpi::util::literals;
  constexpr std::string_view astring = "name";
  constexpr int arrsize = 5;
  constexpr auto str = Concat(
      wpi::util::ct_string<char, std::char_traits<char>, astring.size()>{astring},
      "["_ct_string, wpi::util::NumToCtString<arrsize>(), "]"_ct_string);
  static_assert(str.size() == 7);
  ASSERT_EQ(std::string{str}, "name[5]");
}

TEST(CtStringTest, OperatorPlus) {
  using namespace wpi::util::literals;
  constexpr std::string_view astring = "name";
  constexpr auto str =
      wpi::util::ct_string<char, std::char_traits<char>, astring.size()>{astring} +
      "[]"_ct_string;
  static_assert(str.size() == 6);
  ASSERT_EQ(std::string{str}, "name[]");
}

TEST(CtStringTest, StringViewConversion) {
  using namespace wpi::util::literals;
  constexpr auto str = "[]"_ct_string;
  std::string_view sv = str;
  ASSERT_EQ(sv, "[]");
}
