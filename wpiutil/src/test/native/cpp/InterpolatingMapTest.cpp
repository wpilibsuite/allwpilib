// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/interpolating_map.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>

TEST(InterpolatingMapTest, Insert) {
  wpi::interpolating_map<double, double> table;

  table.insert(125, 450);
  table.insert(200, 510);
  table.insert(268, 525);
  table.insert(312, 550);
  table.insert(326, 650);

  // Key below minimum gives smallest value
  EXPECT_EQ(450, table[100]);

  // Minimum key gives exact value
  EXPECT_EQ(450, table[125]);

  // Key gives interpolated value
  EXPECT_EQ(480, table[162.5]);

  // Key at right of interpolation range gives exact value
  EXPECT_EQ(510, table[200]);

  // Maximum key gives exact value
  EXPECT_EQ(650, table[326]);

  // Key above maximum gives largest value
  EXPECT_EQ(650, table[400]);
}

TEST(InterpolatingMapTest, Clear) {
  wpi::interpolating_map<double, double> table;

  table.insert(125, 450);
  table.insert(200, 510);
  table.insert(268, 525);
  table.insert(312, 550);
  table.insert(326, 650);

  table.clear();

  table.insert(100, 250);
  table.insert(200, 500);

  EXPECT_EQ(375, table[150]);
}
