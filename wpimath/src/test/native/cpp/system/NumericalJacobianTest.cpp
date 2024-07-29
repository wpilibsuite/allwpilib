// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/system/NumericalJacobian.h"

frc::Matrixd<4, 4> A{{1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}, {1, 1, 3, 7}};
frc::Matrixd<4, 2> B{{1, 1}, {2, 1}, {3, 2}, {3, 7}};

// Function from which to recover A and B
frc::Vectord<4> AxBuFn(const frc::Vectord<4>& x, const frc::Vectord<2>& u) {
  return A * x + B * u;
}

// Test that we can recover A from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Ax) {
  frc::Matrixd<4, 4> newA = frc::NumericalJacobianX<4, 4, 2>(
      AxBuFn, frc::Vectord<4>::Zero(), frc::Vectord<2>::Zero());
  EXPECT_TRUE(newA.isApprox(A));
}

// Test that we can recover B from AxBuFn() pretty accurately
TEST(NumericalJacobianTest, Bu) {
  frc::Matrixd<4, 2> newB = frc::NumericalJacobianU<4, 4, 2>(
      AxBuFn, frc::Vectord<4>::Zero(), frc::Vectord<2>::Zero());
  EXPECT_TRUE(newB.isApprox(B));
}

frc::Matrixd<3, 4> C{{1, 2, 4, 1}, {5, 2, 3, 4}, {5, 1, 3, 2}};
frc::Matrixd<3, 2> D{{1, 1}, {2, 1}, {3, 2}};

// Function from which to recover C and D
frc::Vectord<3> CxDuFn(const frc::Vectord<4>& x, const frc::Vectord<2>& u) {
  return C * x + D * u;
}

// Test that we can recover C from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Cx) {
  frc::Matrixd<3, 4> newC = frc::NumericalJacobianX<3, 4, 2>(
      CxDuFn, frc::Vectord<4>::Zero(), frc::Vectord<2>::Zero());
  EXPECT_TRUE(newC.isApprox(C));
}

// Test that we can recover D from CxDuFn() pretty accurately
TEST(NumericalJacobianTest, Du) {
  frc::Matrixd<3, 2> newD = frc::NumericalJacobianU<3, 4, 2>(
      CxDuFn, frc::Vectord<4>::Zero(), frc::Vectord<2>::Zero());
  EXPECT_TRUE(newD.isApprox(D));
}
