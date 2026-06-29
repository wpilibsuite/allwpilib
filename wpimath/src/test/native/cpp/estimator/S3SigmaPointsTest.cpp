// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/S3SigmaPoints.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("S3SigmaPointsTest Simplex", "[wpimath]") {
  constexpr double alpha = 1e-3;
  constexpr double beta = 2;
  constexpr size_t N = 2;

  wpi::math::S3SigmaPoints<N> sigmaPoints{alpha, beta};
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<N>::Zero(), wpi::math::Matrixd<N, N>::Identity());

  auto v1 = points.template block<2, 1>(0, 1);
  auto v2 = points.template block<2, 1>(0, 2);
  auto v3 = points.template block<2, 1>(0, 3);

  CHECK(alpha * std::sqrt(N) == Catch::Approx(v1.norm()));
  CHECK(alpha * std::sqrt(N) == Catch::Approx(v2.norm()));
  CHECK(alpha * std::sqrt(N) == Catch::Approx(v3.norm()));
  CHECK((v1 - v2).norm() == Catch::Approx((v1 - v3).norm()));
  CHECK((v1 - v2).norm() == Catch::Approx((v2 - v3).norm()));
}

TEST_CASE("S3SigmaPointsTest ZeroMean", "[wpimath]") {
  wpi::math::S3SigmaPoints<2> sigmaPoints;
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<2>{0.0, 0.0},
      wpi::math::Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}});

  CHECK((points -
         wpi::math::Matrixd<2, 4>{{0.0, -0.00122474, 0.00122474, 0.0},
                                  {0.0, -0.00070711, -0.00070711, 0.00141421}})
            .norm() < 1e-7);
}

TEST_CASE("S3SigmaPointsTest NonzeroMean", "[wpimath]") {
  wpi::math::S3SigmaPoints<2> sigmaPoints;
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<2>{1.0, 2.0},
      wpi::math::Matrixd<2, 2>{{1.0, 0.0}, {0.0, std::sqrt(10.0)}});

  CHECK((points -
         wpi::math::Matrixd<2, 4>{{1.0, 0.99877526, 1.00122474, 1.0},
                                  {2.0, 1.99776393, 1.99776393, 2.00447214}})
            .norm() < 1e-7);
}
