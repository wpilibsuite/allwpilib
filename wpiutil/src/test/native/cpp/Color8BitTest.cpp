// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Color8Bit.hpp"

#include <string>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

TEST_CASE("Color8BitTest ConstructDefault", "[wpiutil]") {
  constexpr wpi::util::Color8Bit color;

  CHECK(0 == color.red);
  CHECK(0 == color.green);
  CHECK(0 == color.blue);
}

TEST_CASE("Color8BitTest ConstructFromInts", "[wpiutil]") {
  constexpr wpi::util::Color8Bit color{255, 128, 64};

  CHECK(255 == color.red);
  CHECK(128 == color.green);
  CHECK(64 == color.blue);
}

TEST_CASE("Color8BitTest ConstructFromColor", "[wpiutil]") {
  constexpr wpi::util::Color8Bit color{wpi::util::Color{255, 128, 64}};

  CHECK(255 == color.red);
  CHECK(128 == color.green);
  CHECK(64 == color.blue);
}

TEST_CASE("Color8BitTest ConstructFromHexString", "[wpiutil]") {
  constexpr wpi::util::Color8Bit color{"#FF8040"};

  CHECK(255 == color.red);
  CHECK(128 == color.green);
  CHECK(64 == color.blue);

  // No leading #
  CHECK_THROWS_AS(wpi::util::Color8Bit{"112233"}, std::invalid_argument);

  // Too long
  CHECK_THROWS_AS(wpi::util::Color8Bit{"#11223344"}, std::invalid_argument);

  // Invalid hex characters
  CHECK_THROWS_AS(wpi::util::Color8Bit{"#$$$$$$"}, std::invalid_argument);
}

TEST_CASE("Color8BitTest ImplicitConversionToColor", "[wpiutil]") {
  wpi::util::Color color = wpi::util::Color8Bit{255, 128, 64};

  CHECK_THAT(color.red, Catch::Matchers::WithinAbs(1.0, 1e-2));
  CHECK_THAT(color.green, Catch::Matchers::WithinAbs(0.5, 1e-2));
  CHECK_THAT(color.blue, Catch::Matchers::WithinAbs(0.25, 1e-2));
}

TEST_CASE("Color8BitTest ToHexString", "[wpiutil]") {
  constexpr wpi::util::Color8Bit color1{255, 128, 64};
  CHECK("#FF8040" == color1.HexString());

  // Ensure conversion to std::string works
  [[maybe_unused]]
  std::string str = color1.HexString();

  wpi::util::Color8Bit color2{255, 128, 64};
  CHECK("#FF8040" == color2.HexString());
}
