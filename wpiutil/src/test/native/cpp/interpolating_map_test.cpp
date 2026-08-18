// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/interpolating_map.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

TEST_CASE("InterpolatingMapTest Insert", "[wpiutil]") {
  wpi::util::interpolating_map<double, double> table;

  table.insert(125, 450);
  table.insert(200, 510);
  table.insert(268, 525);
  table.insert(312, 550);
  table.insert(326, 650);

  // Key below minimum gives smallest value
  CHECK(450 == table[100]);

  // Minimum key gives exact value
  CHECK(450 == table[125]);

  // Key gives interpolated value
  CHECK(480 == table[162.5]);

  // Key at right of interpolation range gives exact value
  CHECK(510 == table[200]);

  // Maximum key gives exact value
  CHECK(650 == table[326]);

  // Key above maximum gives largest value
  CHECK(650 == table[400]);
}

TEST_CASE("InterpolatingMapTest Clear", "[wpiutil]") {
  wpi::util::interpolating_map<double, double> table;

  table.insert(125, 450);
  table.insert(200, 510);
  table.insert(268, 525);
  table.insert(312, 550);
  table.insert(326, 650);

  table.clear();

  table.insert(100, 250);
  table.insert(200, 500);

  CHECK(375 == table[150]);
}
