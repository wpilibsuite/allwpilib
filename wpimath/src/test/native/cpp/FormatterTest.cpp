// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <fmt/format.h>

#include "frc/fmt/Eigen.h"
#include "frc/fmt/Units.h"
#include "gtest/gtest.h"
#include "units/velocity.h"

TEST(FormatterTest, Eigen) {
  Eigen::Matrix<double, 3, 2> A;
  A << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
  EXPECT_EQ(
      "  1.000000  2.000000\n"
      "  3.000000  4.000000\n"
      "  5.000000  6.000000",
      fmt::format("{}", A));
}

TEST(FormatterTest, Units) {
  EXPECT_EQ("4 m s^-1", fmt::format("{}", 4_mps));
}
