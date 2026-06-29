// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/system/NumericalJacobian.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

wpi::math::Matrixd<4, 4> A{
    {1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}, {1, 1, 3, 7}};
wpi::math::Matrixd<4, 2> B{{1, 1}, {2, 1}, {3, 2}, {3, 7}};

// Function from which to recover A and B
wpi::math::Vectord<4> AxBuFn(const wpi::math::Vectord<4>& x,
                             const wpi::math::Vectord<2>& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST_CASE("NumericalJacobianTest Ax", "[wpimath]") {
  wpi::math::Matrixd<4, 4> newA = wpi::math::NumericalJacobianX<4, 4, 2>(
      AxBuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  CHECK(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST_CASE("NumericalJacobianTest Bu", "[wpimath]") {
  wpi::math::Matrixd<4, 2> newB = wpi::math::NumericalJacobianU<4, 4, 2>(
      AxBuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  CHECK(newB.isApprox(B));
}

Eigen::VectorXd AxBuFn_DynamicSize(const Eigen::VectorXd& x,
                                   const Eigen::VectorXd& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST_CASE("NumericalJacobianTest Ax DynamicSize", "[wpimath]") {
  Eigen::MatrixXd newA = wpi::math::NumericalJacobianX(
      AxBuFn_DynamicSize, wpi::math::Vectord<4>::Zero(),
      wpi::math::Vectord<2>::Zero());
  CHECK(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST_CASE("NumericalJacobianTest Bu DynamicSize", "[wpimath]") {
  Eigen::MatrixXd newB = wpi::math::NumericalJacobianU(
      AxBuFn_DynamicSize, wpi::math::Vectord<4>::Zero(),
      wpi::math::Vectord<2>::Zero());
  CHECK(newB.isApprox(B));
}

wpi::math::Matrixd<3, 4> C{{1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}};
wpi::math::Matrixd<3, 2> D{{1, 1}, {2, 1}, {3, 2}};

// Function from which to recover C and D
wpi::math::Vectord<3> CxDuFn(const wpi::math::Vectord<4>& x,
                             const wpi::math::Vectord<2>& u) {
  return C * x + D * u;
}

// Test that we can recover C from CxDuFn() pretty accurately
TEST_CASE("NumericalJacobianTest Cx", "[wpimath]") {
  wpi::math::Matrixd<3, 4> newC = wpi::math::NumericalJacobianX<3, 4, 2>(
      CxDuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  CHECK(newC.isApprox(C));
}

// Test that we can recover D from CxDuFn() pretty accurately
TEST_CASE("NumericalJacobianTest Du", "[wpimath]") {
  wpi::math::Matrixd<3, 2> newD = wpi::math::NumericalJacobianU<3, 4, 2>(
      CxDuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  CHECK(newD.isApprox(D));
}

Eigen::VectorXd CxDuFn_DynamicSize(const Eigen::VectorXd& x,
                                   const Eigen::VectorXd& u) {
  return C * x + D * u;
}

TEST_CASE("NumericalJacobianTest Cx DynamicSize", "[wpimath]") {
  Eigen::MatrixXd newC = wpi::math::NumericalJacobianX(
      CxDuFn_DynamicSize, Eigen::VectorXd::Zero(4), Eigen::VectorXd::Zero(2));
  CHECK(newC.isApprox(C));
}

TEST_CASE("NumericalJacobianTest Du DynamicSize", "[wpimath]") {
  Eigen::MatrixXd newD = wpi::math::NumericalJacobianU(
      CxDuFn_DynamicSize, Eigen::VectorXd::Zero(4), Eigen::VectorXd::Zero(2));
  CHECK(newD.isApprox(D));
}
