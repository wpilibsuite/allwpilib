// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>

#include "wpi/math/estimator/AngleStatistics.hpp"
#include "wpi/math/linalg/EigenCore.hpp"

TEST(AngleStatisticsTest, Mean) {
  wpi::math::Matrixd<3, 3> sigmas{
      {1, 1.2, 0},
      {359 * std::numbers::pi / 180, 3 * std::numbers::pi / 180, 0},
      {1, 2, 0}};
  // Weights need to produce the mean of the sigmas
  Eigen::Vector3d weights;
  weights.fill(1.0 / sigmas.cols());

  EXPECT_TRUE(Eigen::Vector3d(0.7333333, 0.01163323, 1)
                  .isApprox(wpi::math::AngleMean<3, 3>(sigmas, weights, 1), 1e-3));
}

TEST(AngleStatisticsTest, Mean_DynamicSize) {
  Eigen::MatrixXd sigmas{
      {1, 1.2, 0},
      {359 * std::numbers::pi / 180, 3 * std::numbers::pi / 180, 0},
      {1, 2, 0}};
  // Weights need to produce the mean of the sigmas
  Eigen::VectorXd weights{3};
  weights.fill(1.0 / sigmas.cols());

  EXPECT_TRUE(Eigen::Vector3d(0.7333333, 0.01163323, 1)
                  .isApprox(wpi::math::AngleMean<Eigen::Dynamic, Eigen::Dynamic>(
                                sigmas, weights, 1),
                            1e-3));
}

TEST(AngleStatisticsTest, Residual) {
  Eigen::Vector3d a{1, 1 * std::numbers::pi / 180, 2};
  Eigen::Vector3d b{1, 359 * std::numbers::pi / 180, 1};

  EXPECT_TRUE(wpi::math::AngleResidual<3>(a, b, 1).isApprox(
      Eigen::Vector3d{0, 2 * std::numbers::pi / 180, 1}));
}

TEST(AngleStatisticsTest, Residual_DynamicSize) {
  Eigen::VectorXd a{{1, 1 * std::numbers::pi / 180, 2}};
  Eigen::VectorXd b{{1, 359 * std::numbers::pi / 180, 1}};

  EXPECT_TRUE(wpi::math::AngleResidual<Eigen::Dynamic>(a, b, 1).isApprox(
      Eigen::VectorXd{{0, 2 * std::numbers::pi / 180, 1}}));
}

TEST(AngleStatisticsTest, Add) {
  Eigen::Vector3d a{1, 1 * std::numbers::pi / 180, 2};
  Eigen::Vector3d b{1, 359 * std::numbers::pi / 180, 1};

  EXPECT_TRUE(wpi::math::AngleAdd<3>(a, b, 1).isApprox(Eigen::Vector3d{2, 0, 3}));
}

TEST(AngleStatisticsTest, Add_DynamicSize) {
  Eigen::VectorXd a{{1, 1 * std::numbers::pi / 180, 2}};
  Eigen::VectorXd b{{1, 359 * std::numbers::pi / 180, 1}};

  EXPECT_TRUE(wpi::math::AngleAdd<Eigen::Dynamic>(a, b, 1).isApprox(
      Eigen::VectorXd{{2, 0, 3}}));
}
