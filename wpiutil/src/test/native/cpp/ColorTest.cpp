// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Color.hpp"

#include <string>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "wpi/util/Color8Bit.hpp"

TEST_CASE("ColorTest ConstructDefault", "[wpiutil]") {
  constexpr wpi::util::Color color;

  CHECK_THAT(color.red, Catch::Matchers::WithinULP(0.0, 4));
  CHECK_THAT(color.green, Catch::Matchers::WithinULP(0.0, 4));
  CHECK_THAT(color.blue, Catch::Matchers::WithinULP(0.0, 4));
}

TEST_CASE("ColorTest ConstructFromDoubles", "[wpiutil]") {
  {
    constexpr wpi::util::Color color{1.0, 0.5, 0.25};

    CHECK_THAT(color.red, Catch::Matchers::WithinAbs(1.0, 1e-2));
    CHECK_THAT(color.green, Catch::Matchers::WithinAbs(0.5, 1e-2));
    CHECK_THAT(color.blue, Catch::Matchers::WithinAbs(0.25, 1e-2));
  }

  {
    constexpr wpi::util::Color color{1.0, 0.0, 0.0};

    // Check for exact match to ensure round-and-clamp is correct
    CHECK(1.0 == color.red);
    CHECK(0.0 == color.green);
    CHECK(0.0 == color.blue);
  }
}

TEST_CASE("ColorTest ConstructFromInts", "[wpiutil]") {
  constexpr wpi::util::Color color{255, 128, 64};

  CHECK_THAT(color.red, Catch::Matchers::WithinAbs(1.0, 1e-2));
  CHECK_THAT(color.green, Catch::Matchers::WithinAbs(0.5, 1e-2));
  CHECK_THAT(color.blue, Catch::Matchers::WithinAbs(0.25, 1e-2));
}

TEST_CASE("ColorTest FromHexString", "[wpiutil]") {
  constexpr wpi::util::Color color = wpi::util::Color::FromString("#FF8040");

  CHECK_THAT(color.red, Catch::Matchers::WithinAbs(1.0, 1e-2));
  CHECK_THAT(color.green, Catch::Matchers::WithinAbs(0.5, 1e-2));
  CHECK_THAT(color.blue, Catch::Matchers::WithinAbs(0.25, 1e-2));

  // No leading #
  CHECK_THROWS_AS(wpi::util::Color::FromString("112233"),
                  std::invalid_argument);

  // Too long
  CHECK_THROWS_AS(wpi::util::Color::FromString("#11223344"),
                  std::invalid_argument);

  // Invalid hex characters
  CHECK_THROWS_AS(wpi::util::Color::FromString("#$$$$$$"),
                  std::invalid_argument);
}

TEST_CASE("ColorTest FromRGBString", "[wpiutil]") {
  constexpr wpi::util::Color color =
      wpi::util::Color::FromString("rgb(255, 128, 64)");

  CHECK_THAT(color.red, Catch::Matchers::WithinAbs(1.0, 1e-2));
  CHECK_THAT(color.green, Catch::Matchers::WithinAbs(0.5, 1e-2));
  CHECK_THAT(color.blue, Catch::Matchers::WithinAbs(0.25, 1e-2));

  // Missing rgb()
  CHECK_THROWS_AS(wpi::util::Color::FromString("255, 128, 64"),
                  std::invalid_argument);

  // Too few components
  CHECK_THROWS_AS(wpi::util::Color::FromString("rgb(255, 128)"),
                  std::invalid_argument);

  // Too many components
  CHECK_THROWS_AS(wpi::util::Color::FromString("rgb(255, 128, 64, 32)"),
                  std::invalid_argument);

  // Non-integer component
  CHECK_THROWS_AS(wpi::util::Color::FromString("rgb(255, abc, 64)"),
                  std::invalid_argument);
}

TEST_CASE("ColorTest FromHSV", "[wpiutil]") {
  constexpr wpi::util::Color color = wpi::util::Color::FromHSV(90, 128, 64);

  CHECK_THAT(color.red, Catch::Matchers::WithinULP(0.125732421875, 4));
  CHECK_THAT(color.green, Catch::Matchers::WithinULP(0.251220703125, 4));
  CHECK_THAT(color.blue, Catch::Matchers::WithinULP(0.251220703125, 4));
}

TEST_CASE("ColorTest FromHSVExactRgbValues", "[wpiutil]") {
  struct TestCase {
    int h;
    int s;
    int v;
    int r;
    int g;
    int b;
  };

  constexpr TestCase CASES[] = {
      {0, 0, 0, 0, 0, 0},          {0, 0, 255, 255, 255, 255},
      {0, 255, 255, 255, 0, 0},    {60, 255, 255, 0, 255, 0},
      {120, 255, 255, 0, 0, 255},  {30, 255, 255, 255, 255, 0},
      {90, 255, 255, 0, 255, 255}, {150, 255, 255, 255, 0, 255},
      {0, 255, 128, 128, 0, 0},    {60, 255, 128, 0, 128, 0},
      {120, 255, 128, 0, 0, 128},
  };

  for (const auto& test : CASES) {
    UNSCOPED_INFO("FromHSV(" << test.h << ", " << test.s << ", " << test.v
                             << ")");
    wpi::util::Color8Bit color{
        wpi::util::Color::FromHSV(test.h, test.s, test.v)};
    CHECK(test.r == color.red);
    CHECK(test.g == color.green);
    CHECK(test.b == color.blue);
  }
}

TEST_CASE("ColorTest ToHexString", "[wpiutil]") {
  constexpr wpi::util::Color color1{255, 128, 64};
  CHECK("#FF8040" == color1.HexString());

  // Ensure conversion to std::string works
  [[maybe_unused]]
  std::string str = color1.HexString();

  wpi::util::Color color2{255, 128, 64};
  CHECK("#FF8040" == color2.HexString());
}
