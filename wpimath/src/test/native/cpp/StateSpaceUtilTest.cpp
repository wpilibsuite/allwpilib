// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <array>

#include "frc/EigenCore.h"
#include "frc/StateSpaceUtil.h"
#include "frc/system/NumericalIntegration.h"

TEST(StateSpaceUtilTest, MakeMatrix) {
  // Column vector
  frc::Vectord<2> mat1 = frc::MakeMatrix<2, 1>(1.0, 2.0);
  EXPECT_NEAR(mat1(0), 1.0, 1e-3);
  EXPECT_NEAR(mat1(1), 2.0, 1e-3);

  // Row vector
  Eigen::RowVector<double, 2> mat2 = frc::MakeMatrix<1, 2>(1.0, 2.0);
  EXPECT_NEAR(mat2(0), 1.0, 1e-3);
  EXPECT_NEAR(mat2(1), 2.0, 1e-3);

  // Square matrix
  frc::Matrixd<2, 2> mat3 = frc::MakeMatrix<2, 2>(1.0, 2.0, 3.0, 4.0);
  EXPECT_NEAR(mat3(0, 0), 1.0, 1e-3);
  EXPECT_NEAR(mat3(0, 1), 2.0, 1e-3);
  EXPECT_NEAR(mat3(1, 0), 3.0, 1e-3);
  EXPECT_NEAR(mat3(1, 1), 4.0, 1e-3);

  // Nonsquare matrix with more rows than columns
  frc::Matrixd<3, 2> mat4 = frc::MakeMatrix<3, 2>(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  EXPECT_NEAR(mat4(0, 0), 1.0, 1e-3);
  EXPECT_NEAR(mat4(0, 1), 2.0, 1e-3);
  EXPECT_NEAR(mat4(1, 0), 3.0, 1e-3);
  EXPECT_NEAR(mat4(1, 1), 4.0, 1e-3);
  EXPECT_NEAR(mat4(2, 0), 5.0, 1e-3);
  EXPECT_NEAR(mat4(2, 1), 6.0, 1e-3);

  // Nonsquare matrix with more columns than rows
  frc::Matrixd<2, 3> mat5 = frc::MakeMatrix<2, 3>(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  EXPECT_NEAR(mat5(0, 0), 1.0, 1e-3);
  EXPECT_NEAR(mat5(0, 1), 2.0, 1e-3);
  EXPECT_NEAR(mat5(0, 2), 3.0, 1e-3);
  EXPECT_NEAR(mat5(1, 0), 4.0, 1e-3);
  EXPECT_NEAR(mat5(1, 1), 5.0, 1e-3);
  EXPECT_NEAR(mat5(1, 2), 6.0, 1e-3);
}

TEST(StateSpaceUtilTest, CostParameterPack) {
  frc::Matrixd<3, 3> mat = frc::MakeCostMatrix(1.0, 2.0, 3.0);
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
  frc::Matrixd<3, 3> mat = frc::MakeCostMatrix<3>({1.0, 2.0, 3.0});
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
  frc::Matrixd<3, 3> mat = frc::MakeCovMatrix(1.0, 2.0, 3.0);
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
  frc::Matrixd<3, 3> mat = frc::MakeCovMatrix<3>({1.0, 2.0, 3.0});
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
  frc::Vectord<2> vec = frc::MakeWhiteNoiseVector(2.0, 3.0);
  static_cast<void>(vec);
}

TEST(StateSpaceUtilTest, WhiteNoiseVectorArray) {
  frc::Vectord<2> vec = frc::MakeWhiteNoiseVector<2>({2.0, 3.0});
  static_cast<void>(vec);
}

TEST(StateSpaceUtilTest, IsStabilizable) {
  frc::Matrixd<2, 1> B{0, 1};

  // First eigenvalue is uncontrollable and unstable.
  // Second eigenvalue is controllable and stable.
  EXPECT_FALSE(
      (frc::IsStabilizable<2, 1>(frc::Matrixd<2, 2>{{1.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and marginally stable.
  // Second eigenvalue is controllable and stable.
  EXPECT_FALSE(
      (frc::IsStabilizable<2, 1>(frc::Matrixd<2, 2>{{1, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and stable.
  EXPECT_TRUE(
      (frc::IsStabilizable<2, 1>(frc::Matrixd<2, 2>{{0.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and unstable.
  EXPECT_TRUE(
      (frc::IsStabilizable<2, 1>(frc::Matrixd<2, 2>{{0.2, 0}, {0, 1.2}}, B)));
}

TEST(StateSpaceUtilTest, IsDetectable) {
  frc::Matrixd<1, 2> C{0, 1};

  // First eigenvalue is unobservable and unstable.
  // Second eigenvalue is observable and stable.
  EXPECT_FALSE(
      (frc::IsDetectable<2, 1>(frc::Matrixd<2, 2>{{1.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and marginally stable.
  // Second eigenvalue is observable and stable.
  EXPECT_FALSE(
      (frc::IsDetectable<2, 1>(frc::Matrixd<2, 2>{{1, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and stable.
  EXPECT_TRUE(
      (frc::IsDetectable<2, 1>(frc::Matrixd<2, 2>{{0.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and unstable.
  EXPECT_TRUE(
      (frc::IsDetectable<2, 1>(frc::Matrixd<2, 2>{{0.2, 0}, {0, 1.2}}, C)));
}
