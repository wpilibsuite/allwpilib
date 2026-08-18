// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/StringExtras.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

TEST_CASE("StringExtrasTest RemovePrefix", "[wpiutil]") {
  std::string_view original = "wpilib";
  auto modified = wpi::util::remove_prefix(original, "wpi");
  CHECK(original == "wpilib");
  CHECK(modified == std::optional{"lib"});
}

TEST_CASE("StringExtrasTest RemoveSuffix", "[wpiutil]") {
  std::string_view original = "wpilib";
  auto modified = wpi::util::remove_suffix(original, "lib");
  CHECK(original == "wpilib");
  CHECK(modified == std::optional{"wpi"});
}

TEST_CASE("StringExtrasTest RemovePrefixNoMatch", "[wpiutil]") {
  std::string_view original = "wpilib";
  auto modified = wpi::util::remove_prefix(original, "foo");
  CHECK(original == "wpilib");
  CHECK(modified == std::nullopt);
}

TEST_CASE("StringExtrasTest RemoveSuffixNoMatch", "[wpiutil]") {
  std::string_view original = "wpilib";
  auto modified = wpi::util::remove_suffix(original, "foo");
  CHECK(original == "wpilib");
  CHECK(modified == std::nullopt);
}
