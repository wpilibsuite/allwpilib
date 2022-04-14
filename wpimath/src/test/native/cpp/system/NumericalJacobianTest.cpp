// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/system/NumericalJacobian.h"

Eigen::Matrix<double, 4, 4> A{
    {1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}, {1, 1, 3, 7}};
Eigen::Matrix<double, 4, 2> B{{1, 1}, {2, 1}, {3, 2}, {3, 7}};

// Function from which to recover A and B
Eigen::Vector<double, 4> AxBuFn(const Eigen::Vector<double, 4>& x,
                                const Eigen::Vector<double, 2>& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Ax) {
  Eigen::Matrix<double, 4, 4> newA =
      frc::NumericalJacobianX<4, 4, 2>(AxBuFn, Eigen::Vector<double, 4>::Zero(),
                                       Eigen::Vector<double, 2>::Zero());
  EXPECT_TRUE(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Bu) {
  Eigen::Matrix<double, 4, 2> newB =
      frc::NumericalJacobianU<4, 4, 2>(AxBuFn, Eigen::Vector<double, 4>::Zero(),
                                       Eigen::Vector<double, 2>::Zero());
  EXPECT_TRUE(newB.isApprox(B));
}

Eigen::Matrix<double, 3, 4> C{{1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}};
Eigen::Matrix<double, 3, 2> D{{1, 1}, {2, 1}, {3, 2}};

// Function from which to recover C and D
Eigen::Vector<double, 3> CxDuFn(const Eigen::Vector<double, 4>& x,
                                const Eigen::Vector<double, 2>& u) {
  return C * x + D * u;
}

// Test that we can recover C from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Cx) {
  Eigen::Matrix<double, 3, 4> newC =
      frc::NumericalJacobianX<3, 4, 2>(CxDuFn, Eigen::Vector<double, 4>::Zero(),
                                       Eigen::Vector<double, 2>::Zero());
  EXPECT_TRUE(newC.isApprox(C));
}

// Test that we can recover D from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Du) {
  Eigen::Matrix<double, 3, 2> newD =
      frc::NumericalJacobianU<3, 4, 2>(CxDuFn, Eigen::Vector<double, 4>::Zero(),
                                       Eigen::Vector<double, 2>::Zero());
  EXPECT_TRUE(newD.isApprox(D));
}
