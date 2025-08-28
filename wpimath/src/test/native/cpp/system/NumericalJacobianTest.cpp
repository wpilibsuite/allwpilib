// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/system/NumericalJacobian.h"

wpi::math::Matrixd<4, 4> A{
    {1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}, {1, 1, 3, 7}};
wpi::math::Matrixd<4, 2> B{{1, 1}, {2, 1}, {3, 2}, {3, 7}};

// Function from which to recover A and B
wpi::math::Vectord<4> AxBuFn(const wpi::math::Vectord<4>& x,
                           const wpi::math::Vectord<2>& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Ax) {
  wpi::math::Matrixd<4, 4> newA = wpi::math::NumericalJacobianX<4, 4, 2>(
      AxBuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  EXPECT_TRUE(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Bu) {
  wpi::math::Matrixd<4, 2> newB = wpi::math::NumericalJacobianU<4, 4, 2>(
      AxBuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  EXPECT_TRUE(newB.isApprox(B));
}

Eigen::VectorXd AxBuFn_DynamicSize(const Eigen::VectorXd& x,
                                   const Eigen::VectorXd& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Ax_DynamicSize) {
  Eigen::MatrixXd newA = wpi::math::NumericalJacobianX(
      AxBuFn_DynamicSize, wpi::math::Vectord<4>::Zero(),
      wpi::math::Vectord<2>::Zero());
  EXPECT_TRUE(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Bu_DynamicSize) {
  Eigen::MatrixXd newB = wpi::math::NumericalJacobianU(
      AxBuFn_DynamicSize, wpi::math::Vectord<4>::Zero(),
      wpi::math::Vectord<2>::Zero());
  EXPECT_TRUE(newB.isApprox(B));
}

wpi::math::Matrixd<3, 4> C{{1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}};
wpi::math::Matrixd<3, 2> D{{1, 1}, {2, 1}, {3, 2}};

// Function from which to recover C and D
wpi::math::Vectord<3> CxDuFn(const wpi::math::Vectord<4>& x,
                           const wpi::math::Vectord<2>& u) {
  return C * x + D * u;
}

// Test that we can recover C from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Cx) {
  wpi::math::Matrixd<3, 4> newC = wpi::math::NumericalJacobianX<3, 4, 2>(
      CxDuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  EXPECT_TRUE(newC.isApprox(C));
}

// Test that we can recover D from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Du) {
  wpi::math::Matrixd<3, 2> newD = wpi::math::NumericalJacobianU<3, 4, 2>(
      CxDuFn, wpi::math::Vectord<4>::Zero(), wpi::math::Vectord<2>::Zero());
  EXPECT_TRUE(newD.isApprox(D));
}

Eigen::VectorXd CxDuFn_DynamicSize(const Eigen::VectorXd& x,
                                   const Eigen::VectorXd& u) {
  return C * x + D * u;
}

TEST(NumericalJacobianTest, Cx_DynamicSize) {
  Eigen::MatrixXd newC = wpi::math::NumericalJacobianX(
      CxDuFn_DynamicSize, Eigen::VectorXd::Zero(4), Eigen::VectorXd::Zero(2));
  EXPECT_TRUE(newC.isApprox(C));
}

TEST(NumericalJacobianTest, Du_DynamicSize) {
  Eigen::MatrixXd newD = wpi::math::NumericalJacobianU(
      CxDuFn_DynamicSize, Eigen::VectorXd::Zero(4), Eigen::VectorXd::Zero(2));
  EXPECT_TRUE(newD.isApprox(D));
}
