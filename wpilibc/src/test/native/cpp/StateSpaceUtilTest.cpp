/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <array>

#include <Eigen/Core>

#include "frc/StateSpaceUtil.h"
#include "frc/system/RungeKutta.h"

TEST(StateSpaceUtilTest, CostParameterPack) {
  Eigen::Matrix<double, 3, 3> mat = frc::MakeCostMatrix(1.0, 2.0, 3.0);
  EXPECT_NEAR(mat(0, 0), 1.0, 1e-3);
  EXPECT_NEAR(mat(0, 1), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 0), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 1), 1.0 / 4.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(2, 2), 1.0 / 9.0, 1e-3);
}

TEST(StateSpaceUtilTest, CostArray) {
  Eigen::Matrix<double, 3, 3> mat = frc::MakeCostMatrix<3>({1.0, 2.0, 3.0});
  EXPECT_NEAR(mat(0, 0), 1.0, 1e-3);
  EXPECT_NEAR(mat(0, 1), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 0), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 1), 1.0 / 4.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(2, 2), 1.0 / 9.0, 1e-3);
}

TEST(StateSpaceUtilTest, CovParameterPack) {
  Eigen::Matrix<double, 3, 3> mat = frc::MakeCovMatrix(1.0, 2.0, 3.0);
  EXPECT_NEAR(mat(0, 0), 1.0, 1e-3);
  EXPECT_NEAR(mat(0, 1), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 0), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 1), 4.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(2, 2), 9.0, 1e-3);
}

TEST(StateSpaceUtilTest, CovArray) {
  Eigen::Matrix<double, 3, 3> mat = frc::MakeCovMatrix<3>({1.0, 2.0, 3.0});
  EXPECT_NEAR(mat(0, 0), 1.0, 1e-3);
  EXPECT_NEAR(mat(0, 1), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 0), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 1), 4.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(0, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(1, 2), 0.0, 1e-3);
  EXPECT_NEAR(mat(2, 2), 9.0, 1e-3);
}

TEST(StateSpaceUtilTest, WhiteNoiseVectorParameterPack) {
  Eigen::Matrix<double, 2, 1> vec = frc::MakeWhiteNoiseVector(2.0, 3.0);
  static_cast<void>(vec);
}

TEST(StateSpaceUtilTest, WhiteNoiseVectorArray) {
  Eigen::Matrix<double, 2, 1> vec = frc::MakeWhiteNoiseVector<2>({2.0, 3.0});
  static_cast<void>(vec);
}

TEST(StateSpaceUtilTest, IsStabilizable) {
  Eigen::Matrix<double, 2, 2> A;
  Eigen::Matrix<double, 2, 1> B;
  B << 0, 1;

  // First eigenvalue is uncontrollable and unstable.
  // Second eigenvalue is controllable and stable.
  A << 1.2, 0, 0, 0.5;
  EXPECT_FALSE(frc::IsStabilizable(A, B));

  // First eigenvalue is uncontrollable and marginally stable.
  // Second eigenvalue is controllable and stable.
  A << 1, 0, 0, 0.5;
  EXPECT_FALSE(frc::IsStabilizable(A, B));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and stable.
  A << 0.2, 0, 0, 0.5;
  EXPECT_TRUE(frc::IsStabilizable(A, B));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and unstable.
  A << 0.2, 0, 0, 1.2;
  EXPECT_TRUE(frc::IsStabilizable(A, B));
}
