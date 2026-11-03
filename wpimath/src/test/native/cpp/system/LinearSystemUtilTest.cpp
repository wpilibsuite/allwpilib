// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/LinearSystemUtil.hpp"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LinearSystemUtilTest IsStabilizable", "[wpimath]") {
  Eigen::Matrix<double, 2, 1> B1{0, 1};

  // First eigenvalue is uncontrollable and unstable.
  // Second eigenvalue is controllable and stable.
  CHECK_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1.2, 0}, {0, 0.5}}, B1)));

  // First eigenvalue is uncontrollable and marginally stable.
  // Second eigenvalue is controllable and stable.
  CHECK_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1, 0}, {0, 0.5}}, B1)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and stable.
  CHECK((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 0.5}}, B1)));

  // First eigenvalue is uncontrollable and stable.
  // Second eigenvalue is controllable and unstable.
  CHECK((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 1.2}}, B1)));

  // Controllable stable complex eigenvalues (i and -i)
  CHECK((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0, 1}, {-1, 0}}, B1)));

  Eigen::Matrix<double, 2, 1> B2{0, 0};

  // Uncontrollable stable complex eigenvalues (i and -i)
  CHECK_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0, 1}, {-1, 0}}, B2)));

  // Uncontrollable stable complex eigenvalues (0.5 + √(3)/2i and 0.5 - √(3)/2i)
  CHECK_FALSE((wpi::math::IsStabilizable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0, -1}, {1, 1}}, B2)));
}

TEST_CASE("LinearSystemUtilTest IsDetectable", "[wpimath]") {
  Eigen::Matrix<double, 1, 2> C1{0, 1};

  // First eigenvalue is unobservable and unstable.
  // Second eigenvalue is observable and stable.
  CHECK_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1.2, 0}, {0, 0.5}}, C1)));

  // First eigenvalue is unobservable and marginally stable.
  // Second eigenvalue is observable and stable.
  CHECK_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{1, 0}, {0, 0.5}}, C1)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and stable.
  CHECK((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 0.5}}, C1)));

  // First eigenvalue is unobservable and stable.
  // Second eigenvalue is observable and unstable.
  CHECK((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0.2, 0}, {0, 1.2}}, C1)));

  // Detectable stable complex eigenvalues (i and -i)
  CHECK((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0, 1}, {-1, 0}}, C1)));

  Eigen::Matrix<double, 1, 2> C2{0, 0};

  // Undetectable stable complex eigenvalues (i and -i)
  CHECK_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0, 1}, {-1, 0}}, C2)));

  // Undetectable stable complex eigenvalues (0.5 + √(3)/2i and 0.5 - √(3)/2i)
  CHECK_FALSE((wpi::math::IsDetectable<2, 1>(
      Eigen::Matrix<double, 2, 2>{{0, -1}, {1, 1}}, C2)));
}
