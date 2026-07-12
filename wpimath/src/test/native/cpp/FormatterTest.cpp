// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <format>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/fmt/Eigen.hpp"
#include "wpi/units/velocity.hpp"

TEST_CASE("FormatterTest Eigen", "[wpimath]") {
  Eigen::Matrix<double, 3, 2> A{{0.0, 1.0}, {2.0, 3.0}, {4.0, 5.0}};
  CHECK(
      "  0.000000  1.000000\n"
      "  2.000000  3.000000\n"
      "  4.000000  5.000000" == std::format("{:f}", A));

  Eigen::MatrixXd B{{0.0, 1.0}, {2.0, 3.0}, {4.0, 5.0}};
  CHECK(
      "  0.000000  1.000000\n"
      "  2.000000  3.000000\n"
      "  4.000000  5.000000" == std::format("{:f}", B));

  Eigen::Array2d C{0.0, 1.0};
  CHECK("  0.000000\n  1.000000" == std::format("{:f}", C));

  Eigen::SparseMatrix<double> D{3, 2};
  std::vector<Eigen::Triplet<double>> triplets;
  triplets.emplace_back(0, 1, 1.0);
  triplets.emplace_back(1, 0, 2.0);
  triplets.emplace_back(1, 1, 3.0);
  triplets.emplace_back(2, 0, 4.0);
  triplets.emplace_back(2, 1, 5.0);
  D.setFromTriplets(triplets.begin(), triplets.end());
  CHECK(
      "  0.000000  1.000000\n"
      "  2.000000  3.000000\n"
      "  4.000000  5.000000" == std::format("{:f}", D));
}

TEST_CASE("FormatterTest Units", "[wpimath]") {
  CHECK("4 mps" == std::format("{}", 4_mps));
}
