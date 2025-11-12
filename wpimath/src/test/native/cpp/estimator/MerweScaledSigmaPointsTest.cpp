// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/MerweScaledSigmaPoints.hpp"

#include <gtest/gtest.h>

TEST(MerweScaledSigmaPointsTest, ZeroMean) {
  wpi::math::MerweScaledSigmaPoints<2> sigmaPoints;
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<2>{0.0, 0.0},
      wpi::math::Matrixd<2, 2>{{1.0, 0.0}, {0.0, 1.0}});

  EXPECT_TRUE(
      (points -
       wpi::math::Matrixd<2, 5>{{0.0, 0.00173205, 0.0, -0.00173205, 0.0},
                                {0.0, 0.0, 0.00173205, 0.0, -0.00173205}})
          .norm() < 1e-3);
}

TEST(MerweScaledSigmaPointsTest, NonzeroMean) {
  wpi::math::MerweScaledSigmaPoints<2> sigmaPoints;
  auto points = sigmaPoints.SquareRootSigmaPoints(
      wpi::math::Vectord<2>{1.0, 2.0},
      wpi::math::Matrixd<2, 2>{{1.0, 0.0}, {0.0, std::sqrt(10.0)}});

  EXPECT_TRUE(
      (points - wpi::math::Matrixd<2, 5>{{1.0, 1.00173205, 1.0, 0.998268, 1.0},
                                         {2.0, 2.0, 2.00548, 2.0, 1.99452}})
          .norm() < 1e-3);
}
