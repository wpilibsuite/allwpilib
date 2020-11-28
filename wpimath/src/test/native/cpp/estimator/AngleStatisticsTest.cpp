/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include "Eigen/Core"
#include "frc/estimator/AngleStatistics.h"

TEST(AngleStatisticsTest, TestMean) {
  Eigen::Matrix<double, 3, 3> sigmas;
  sigmas << 1, 1.2, 0, 359 * wpi::math::pi / 180, 3 * wpi::math::pi / 180, 0, 1,
      2, 0;
  // Weights need to produce the mean of the sigmas
  Eigen::Vector3d weights{};
  weights.fill(1.0 / sigmas.cols());

  EXPECT_TRUE(Eigen::Vector3d(0.7333333, 0.01163323, 1)
                  .isApprox(frc::AngleMean<3, 1>(sigmas, weights, 1), 1e-3));
}

TEST(AngleStatisticsTest, TestResidual) {
  Eigen::Vector3d a(1, 1 * wpi::math::pi / 180, 2);
  Eigen::Vector3d b(1, 359 * wpi::math::pi / 180, 1);

  EXPECT_TRUE(frc::AngleResidual<3>(a, b, 1).isApprox(
      Eigen::Vector3d(0, 2 * wpi::math::pi / 180, 1)));
}

TEST(AngleStatisticsTest, TestAdd) {
  Eigen::Vector3d a(1, 1 * wpi::math::pi / 180, 2);
  Eigen::Vector3d b(1, 359 * wpi::math::pi / 180, 1);

  EXPECT_TRUE(frc::AngleAdd<3>(a, b, 1).isApprox(Eigen::Vector3d(2, 0, 3)));
}

TEST(AngleStatisticsTest, TestNormalize) {
  EXPECT_NEAR(frc::NormalizeAngle(-2000 * wpi::math::pi / 180),
              160 * wpi::math::pi / 180, 1e-6);
  EXPECT_NEAR(frc::NormalizeAngle(358 * wpi::math::pi / 180),
              -2 * wpi::math::pi / 180, 1e-6);
  EXPECT_NEAR(frc::NormalizeAngle(360 * wpi::math::pi / 180), 0, 1e-6);
}
