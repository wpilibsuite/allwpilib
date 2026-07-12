// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/LinearSystemUtil.hpp"

#include <Eigen/Core>
#include <gtest/gtest.h>

TEST(LinearSystemUtilTest, IsStabilizable) {
  Eigen::Matrix<double, 2, 1> B{0, 1};

  // First eigenvalue is uncontrollable and unstable.
  // Second eigenvalue is controllable and stable.
  EXPECT_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and marginally stable.
  // Second eigenvalue is controllable and stable.
  EXPECT_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and stable.
  EXPECT_TRUE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and unstable.
  EXPECT_TRUE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 1.2}}, B)));
}

TEST(LinearSystemUtilTest, IsDetectable) {
  Eigen::Matrix<double, 1, 2> C{0, 1};

  // First eigenvalue is unobservable and unstable.
  // Second eigenvalue is observable and stable.
  EXPECT_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and marginally stable.
  // Second eigenvalue is observable and stable.
  EXPECT_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and stable.
  EXPECT_TRUE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and unstable.
  EXPECT_TRUE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 1.2}}, C)));
}
