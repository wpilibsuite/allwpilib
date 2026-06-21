// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/LinearSystemUtil.hpp"

#include <Eigen/Core>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LinearSystemUtilTest IsStabilizable", "[wpimath]") {
  Eigen::Matrix<double, 2, 1> B{0, 1};

  // First eigenvalue is uncontrollable and unstable.
  // Second eigenvalue is controllable and stable.
  CHECK_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and marginally stable.
  // Second eigenvalue is controllable and stable.
  CHECK_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and stable.
  CHECK((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 0.5}}, B)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and unstable.
  CHECK((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 1.2}}, B)));
}

TEST_CASE("LinearSystemUtilTest IsDetectable", "[wpimath]") {
  Eigen::Matrix<double, 1, 2> C{0, 1};

  // First eigenvalue is unobservable and unstable.
  // Second eigenvalue is observable and stable.
  CHECK_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and marginally stable.
  // Second eigenvalue is observable and stable.
  CHECK_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and stable.
  CHECK((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 0.5}}, C)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and unstable.
  CHECK((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 1.2}}, C)));
}
