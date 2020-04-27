/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include "frc/system/NumericalJacobian.h"

Eigen::Matrix<double, 4, 4> A = (Eigen::Matrix<double, 4, 4>() << 1, 2, 4, 1, 5,
                                 2, 3, 4, 5, 1, 3, 2, 1, 1, 3, 7)
                                    .finished();

Eigen::Matrix<double, 4, 2> B =
    (Eigen::Matrix<double, 4, 2>() << 1, 1, 2, 1, 3, 2, 3, 7).finished();

// Function from which to recover A and B
Eigen::Matrix<double, 4, 1> AxBuFn(const Eigen::Matrix<double, 4, 1>& x,
                                   const Eigen::Matrix<double, 2, 1>& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Ax) {
  Eigen::Matrix<double, 4, 4> newA = frc::NumericalJacobianX<4, 4, 2>(
      AxBuFn, Eigen::Matrix<double, 4, 1>::Zero(),
      Eigen::Matrix<double, 2, 1>::Zero());
  EXPECT_TRUE(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Bu) {
  Eigen::Matrix<double, 4, 2> newB = frc::NumericalJacobianU<4, 4, 2>(
      AxBuFn, Eigen::Matrix<double, 4, 1>::Zero(),
      Eigen::Matrix<double, 2, 1>::Zero());
  EXPECT_TRUE(newB.isApprox(B));
}

Eigen::Matrix<double, 3, 4> C =
    (Eigen::Matrix<double, 3, 4>() << 1, 2, 4, 1, 5, 2, 3, 4, 5, 1, 3, 2)
        .finished();

Eigen::Matrix<double, 3, 2> D =
    (Eigen::Matrix<double, 3, 2>() << 1, 1, 2, 1, 3, 2).finished();

// Function from which to recover C and D
Eigen::Matrix<double, 3, 1> CxDuFn(const Eigen::Matrix<double, 4, 1>& x,
                                   const Eigen::Matrix<double, 2, 1>& u) {
  return C * x + D * u;
}

// Test that we can recover C from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Cx) {
  Eigen::Matrix<double, 3, 4> newC = frc::NumericalJacobianX<3, 4, 2>(
      CxDuFn, Eigen::Matrix<double, 4, 1>::Zero(),
      Eigen::Matrix<double, 2, 1>::Zero());
  EXPECT_TRUE(newC.isApprox(C));
}

// Test that we can recover D from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Du) {
  Eigen::Matrix<double, 3, 2> newD = frc::NumericalJacobianU<3, 4, 2>(
      CxDuFn, Eigen::Matrix<double, 4, 1>::Zero(),
      Eigen::Matrix<double, 2, 1>::Zero());
  EXPECT_TRUE(newD.isApprox(D));
}
