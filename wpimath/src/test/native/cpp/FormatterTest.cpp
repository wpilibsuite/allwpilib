// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <fmt/format.h>

#include "frc/fmt/Eigen.h"
#include "frc/fmt/Units.h"
#include "gtest/gtest.h"
#include "units/velocity.h"

TEST(FormatterTest, Eigen) {
  Eigen::Matrix<double, 3, 2> A{{0.0, 1.0}, {2.0, 3.0}, {4.0, 5.0}};
  EXPECT_EQ(
      "  0.000000  1.000000\n"
      "  2.000000  3.000000\n"
      "  4.000000  5.000000",
      fmt::format("{}", A));

  Eigen::MatrixXd B{{0.0, 1.0}, {2.0, 3.0}, {4.0, 5.0}};
  EXPECT_EQ(
      "  0.000000  1.000000\n"
      "  2.000000  3.000000\n"
      "  4.000000  5.000000",
      fmt::format("{}", B));

  Eigen::SparseMatrix<double> C{3, 2};
  std::vector<Eigen::Triplet<double>> triplets;
  triplets.emplace_back(0, 1, 1.0);
  triplets.emplace_back(1, 0, 2.0);
  triplets.emplace_back(1, 1, 3.0);
  triplets.emplace_back(2, 0, 4.0);
  triplets.emplace_back(2, 1, 5.0);
  C.setFromTriplets(triplets.begin(), triplets.end());
  EXPECT_EQ(
      "  0.000000  1.000000\n"
      "  2.000000  3.000000\n"
      "  4.000000  5.000000",
      fmt::format("{}", C));
}

TEST(FormatterTest, Units) {
  EXPECT_EQ("4 mps", fmt::format("{}", 4_mps));
}
