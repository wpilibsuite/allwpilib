// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/linalg/detail/MatrixBalance.hpp"

#include <cmath>
#include <limits>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("MatrixBalanceTest RoundHalfToEvenDiv2", "[wpimath]") {
  CHECK(wpi::math::detail::RoundHalfToEvenDiv2(0) == 0);
  CHECK(wpi::math::detail::RoundHalfToEvenDiv2(1) == 0);
  CHECK(wpi::math::detail::RoundHalfToEvenDiv2(3) == 2);
  CHECK(wpi::math::detail::RoundHalfToEvenDiv2(5) == 2);

  CHECK(wpi::math::detail::RoundHalfToEvenDiv2(-1) == 0);
  CHECK(wpi::math::detail::RoundHalfToEvenDiv2(-3) == -2);
  CHECK(wpi::math::detail::RoundHalfToEvenDiv2(-5) == -2);
}

TEST_CASE("MatrixBalanceTest IgnoresUnsupportedMatrices", "[wpimath]") {
  Eigen::MatrixXd nonsquare = Eigen::MatrixXd::Ones(2, 3);
  auto nonsquareScales = wpi::math::detail::BalanceMatrixPowerOfTwo(nonsquare);
  REQUIRE(nonsquareScales.size() == 2);
  CHECK(nonsquareScales[0] == 0);
  CHECK(nonsquareScales[1] == 0);

  Eigen::Matrix2d zero = Eigen::Matrix2d::Zero();
  auto zeroScales = wpi::math::detail::BalanceMatrixPowerOfTwo(zero);
  CHECK(zeroScales[0] == 0);
  CHECK(zeroScales[1] == 0);

  Eigen::Matrix2d nonfinite;
  nonfinite << 0.0, std::numeric_limits<double>::infinity(), 1.0, 0.0;
  auto nonfiniteScales = wpi::math::detail::BalanceMatrixPowerOfTwo(nonfinite);
  CHECK(nonfiniteScales[0] == 0);
  CHECK(nonfiniteScales[1] == 0);
}

TEST_CASE("MatrixBalanceTest BalancesByPowersOfTwo", "[wpimath]") {
  Eigen::Matrix2d matrix;
  matrix << 0.0, std::ldexp(1.0, 20), std::ldexp(1.0, -20), 0.0;

  auto scales = wpi::math::detail::BalanceMatrixPowerOfTwo(matrix);
  CHECK(scales[0] == 20);
  CHECK(scales[1] == 0);

  Eigen::Matrix2d balanced;
  for (int row = 0; row < matrix.rows(); ++row) {
    for (int col = 0; col < matrix.cols(); ++col) {
      balanced(row, col) =
          std::ldexp(matrix(row, col), scales[col] - scales[row]);
    }
  }

  CHECK(balanced(0, 1) == 1.0);
  CHECK(balanced(1, 0) == 1.0);
}

TEST_CASE("MatrixBalanceTest KeepsExponentsBounded", "[wpimath]") {
  Eigen::Matrix2d matrix;
  matrix << 0.0, std::ldexp(1.0, 968), std::ldexp(1.0, -968), 0.0;

  auto scales = wpi::math::detail::BalanceMatrixPowerOfTwo(matrix);

  constexpr int minScaleExponent = std::numeric_limits<double>::min_exponent +
                                   std::numeric_limits<double>::digits - 2;
  constexpr int maxScaleExponent = -minScaleExponent;

  for (int i = 0; i < scales.size(); ++i) {
    CHECK(scales[i] > minScaleExponent);
    CHECK(scales[i] < maxScaleExponent);
  }
}
