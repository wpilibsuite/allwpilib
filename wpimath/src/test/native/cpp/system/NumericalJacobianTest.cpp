// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/system/NumericalJacobian.h"

wpimath::Matrixd<4, 4> A{
    {1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}, {1, 1, 3, 7}};
wpimath::Matrixd<4, 2> B{{1, 1}, {2, 1}, {3, 2}, {3, 7}};

// Function from which to recover A and B
wpimath::Vectord<4> AxBuFn(const wpimath::Vectord<4>& x,
                           const wpimath::Vectord<2>& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Ax) {
  wpimath::Matrixd<4, 4> newA = wpimath::NumericalJacobianX<4, 4, 2>(
      AxBuFn, wpimath::Vectord<4>::Zero(), wpimath::Vectord<2>::Zero());
  EXPECT_TRUE(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Bu) {
  wpimath::Matrixd<4, 2> newB = wpimath::NumericalJacobianU<4, 4, 2>(
      AxBuFn, wpimath::Vectord<4>::Zero(), wpimath::Vectord<2>::Zero());
  EXPECT_TRUE(newB.isApprox(B));
}

Eigen::VectorXd AxBuFn_DynamicSize(const Eigen::VectorXd& x,
                                   const Eigen::VectorXd& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Ax_DynamicSize) {
  Eigen::MatrixXd newA = wpimath::NumericalJacobianX(
      AxBuFn_DynamicSize, wpimath::Vectord<4>::Zero(),
      wpimath::Vectord<2>::Zero());
  EXPECT_TRUE(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Bu_DynamicSize) {
  Eigen::MatrixXd newB = wpimath::NumericalJacobianU(
      AxBuFn_DynamicSize, wpimath::Vectord<4>::Zero(),
      wpimath::Vectord<2>::Zero());
  EXPECT_TRUE(newB.isApprox(B));
}

wpimath::Matrixd<3, 4> C{{1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}};
wpimath::Matrixd<3, 2> D{{1, 1}, {2, 1}, {3, 2}};

// Function from which to recover C and D
wpimath::Vectord<3> CxDuFn(const wpimath::Vectord<4>& x,
                           const wpimath::Vectord<2>& u) {
  return C * x + D * u;
}

// Test that we can recover C from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Cx) {
  wpimath::Matrixd<3, 4> newC = wpimath::NumericalJacobianX<3, 4, 2>(
      CxDuFn, wpimath::Vectord<4>::Zero(), wpimath::Vectord<2>::Zero());
  EXPECT_TRUE(newC.isApprox(C));
}

// Test that we can recover D from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Du) {
  wpimath::Matrixd<3, 2> newD = wpimath::NumericalJacobianU<3, 4, 2>(
      CxDuFn, wpimath::Vectord<4>::Zero(), wpimath::Vectord<2>::Zero());
  EXPECT_TRUE(newD.isApprox(D));
}

Eigen::VectorXd CxDuFn_DynamicSize(const Eigen::VectorXd& x,
                                   const Eigen::VectorXd& u) {
  return C * x + D * u;
}

TEST(NumericalJacobianTest, Cx_DynamicSize) {
  Eigen::MatrixXd newC = wpimath::NumericalJacobianX(
      CxDuFn_DynamicSize, Eigen::VectorXd::Zero(4), Eigen::VectorXd::Zero(2));
  EXPECT_TRUE(newC.isApprox(C));
}

TEST(NumericalJacobianTest, Du_DynamicSize) {
  Eigen::MatrixXd newD = wpimath::NumericalJacobianU(
      CxDuFn_DynamicSize, Eigen::VectorXd::Zero(4), Eigen::VectorXd::Zero(2));
  EXPECT_TRUE(newD.isApprox(D));
}
