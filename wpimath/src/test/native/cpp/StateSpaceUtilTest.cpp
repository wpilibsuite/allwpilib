// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/math/EigenCore.h>
#include <wpi/math/StateSpaceUtil.h>

TEST(StateSpaceUtilTest, CostParameterPack) {
  constexpr wpi::math::Matrixd<3, 3> mat =
      wpi::math::MakeCostMatrix(1.0, 2.0, 3.0);
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
  constexpr wpi::math::Matrixd<3, 3> mat =
      wpi::math::MakeCostMatrix<3>({1.0, 2.0, 3.0});
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

TEST(StateSpaceUtilTest, CostDynamic) {
  Eigen::MatrixXd mat = wpi::math::MakeCostMatrix(std::vector{1.0, 2.0, 3.0});
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
  constexpr wpi::math::Matrixd<3, 3> mat =
      wpi::math::MakeCovMatrix(1.0, 2.0, 3.0);
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
  constexpr wpi::math::Matrixd<3, 3> mat =
      wpi::math::MakeCovMatrix<3>({1.0, 2.0, 3.0});
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

TEST(StateSpaceUtilTest, CovDynamic) {
  Eigen::MatrixXd mat = wpi::math::MakeCovMatrix(std::vector{1.0, 2.0, 3.0});
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
  [[maybe_unused]]
  wpi::math::Vectord<2> vec = wpi::math::MakeWhiteNoiseVector(2.0, 3.0);
}

TEST(StateSpaceUtilTest, WhiteNoiseVectorArray) {
  [[maybe_unused]]
  wpi::math::Vectord<2> vec = wpi::math::MakeWhiteNoiseVector<2>({2.0, 3.0});
}

TEST(StateSpaceUtilTest, WhiteNoiseVectorDynamic) {
  [[maybe_unused]]
  Eigen::VectorXd vec = wpi::math::MakeWhiteNoiseVector(std::vector{2.0, 3.0});
}

TEST(StateSpaceUtilTest, IsStabilizable) {
  wpi::math::Matrixd<2, 1> B{0, 1};

  // First eigenvalue is uncontrollable and unstable.
  // Second eigenvalue is controllable and stable.
  EXPECT_FALSE((wpi::math::IsStabilizable<2, 1>(
      wpi::math::Matrixd<2, 2>{{1.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and marginally stable.
  // Second eigenvalue is controllable and stable.
  EXPECT_FALSE((wpi::math::IsStabilizable<2, 1>(
      wpi::math::Matrixd<2, 2>{{1, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and stable.
  EXPECT_TRUE((wpi::math::IsStabilizable<2, 1>(
      wpi::math::Matrixd<2, 2>{{0.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and unstable.
  EXPECT_TRUE((wpi::math::IsStabilizable<2, 1>(
      wpi::math::Matrixd<2, 2>{{0.2, 0}, {0, 1.2}}, B)));
}

TEST(StateSpaceUtilTest, IsDetectable) {
  wpi::math::Matrixd<1, 2> C{0, 1};

  // First eigenvalue is unobservable and unstable.
  // Second eigenvalue is observable and stable.
  EXPECT_FALSE((wpi::math::IsDetectable<2, 1>(
      wpi::math::Matrixd<2, 2>{{1.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and marginally stable.
  // Second eigenvalue is observable and stable.
  EXPECT_FALSE((wpi::math::IsDetectable<2, 1>(
      wpi::math::Matrixd<2, 2>{{1, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and stable.
  EXPECT_TRUE((wpi::math::IsDetectable<2, 1>(
      wpi::math::Matrixd<2, 2>{{0.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and unstable.
  EXPECT_TRUE((wpi::math::IsDetectable<2, 1>(
      wpi::math::Matrixd<2, 2>{{0.2, 0}, {0, 1.2}}, C)));
}
