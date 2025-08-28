// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/math/estimator/S3SigmaPoints.h>

TEST(S3SigmaPointsTest, Simplex) {
  constexpr double alpha = 1e-3;
  constexpr double beta = 2;
  constexpr size_t N = 2;

  wpi::math::S3SigmaPoints<N> sigmaPoints{alpha, beta};
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<N>::Zero(), wpi::math::Matrixd<N, N>::Identity());

  auto v1 = points.template block<2, 1>(0, 1);
  auto v2 = points.template block<2, 1>(0, 2);
  auto v3 = points.template block<2, 1>(0, 3);

  EXPECT_DOUBLE_EQ(alpha * std::sqrt(N), v1.norm());
  EXPECT_DOUBLE_EQ(alpha * std::sqrt(N), v2.norm());
  EXPECT_DOUBLE_EQ(alpha * std::sqrt(N), v3.norm());
  EXPECT_DOUBLE_EQ((v1 - v2).norm(), (v1 - v3).norm());
  EXPECT_DOUBLE_EQ((v1 - v2).norm(), (v2 - v3).norm());
}

TEST(S3SigmaPointsTest, ZeroMean) {
  wpi::math::S3SigmaPoints<2> sigmaPoints;
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<2>{0.0, 0.0},
      wpi::math::Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}});

  EXPECT_TRUE(
      (points -
       wpi::math::Matrixd<2, 4>{{0.0, -0.00122474, 0.00122474, 0.0},
                                {0.0, -0.00070711, -0.00070711, 0.00141421}})
          .norm() < 1e-7);
}

TEST(S3SigmaPointsTest, NonzeroMean) {
  wpi::math::S3SigmaPoints<2> sigmaPoints;
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<2>{1.0, 2.0},
      wpi::math::Matrixd<2, 2>{{1.0, 0.0}, {0.0, std::sqrt(10.0)}});

  EXPECT_TRUE(
      (points -
       wpi::math::Matrixd<2, 4>{{1.0, 0.99877526, 1.00122474, 1.0},
                                {2.0, 1.99776393, 1.99776393, 2.00447214}})
          .norm() < 1e-7);
}
