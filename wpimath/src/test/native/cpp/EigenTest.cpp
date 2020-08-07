/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Eigen/Core>
#include <Eigen/LU>

#include "gtest/gtest.h"

TEST(EigenTest, MultiplicationTest) {
  Eigen::Matrix<double, 2, 2> m1;
  m1 << 2, 1, 0, 1;

  Eigen::Matrix<double, 2, 2> m2;
  m2 << 3, 0, 0, 2.5;

  const auto result = m1 * m2;

  Eigen::Matrix<double, 2, 2> expectedResult;
  expectedResult << 6.0, 2.5, 0.0, 2.5;

  EXPECT_TRUE(expectedResult.isApprox(result));

  Eigen::Matrix<double, 2, 3> m3;
  m3 << 1.0, 3.0, 0.5, 2.0, 4.3, 1.2;

  Eigen::Matrix<double, 3, 4> m4;
  m4 << 3.0, 1.5, 2.0, 4.5, 2.3, 1.0, 1.6, 3.1, 5.2, 2.1, 2.0, 1.0;

  const auto result2 = m3 * m4;

  Eigen::Matrix<double, 2, 4> expectedResult2;
  expectedResult2 << 12.5, 5.55, 7.8, 14.3, 22.13, 9.82, 13.28, 23.53;

  EXPECT_TRUE(expectedResult2.isApprox(result2));
}

TEST(EigenTest, TransposeTest) {
  Eigen::Matrix<double, 3, 1> vec;
  vec << 1, 2, 3;

  const auto transpose = vec.transpose();

  Eigen::Matrix<double, 1, 3> expectedTranspose;
  expectedTranspose << 1, 2, 3;

  EXPECT_TRUE(expectedTranspose.isApprox(transpose));
}

TEST(EigenTest, InverseTest) {
  Eigen::Matrix<double, 3, 3> mat;
  mat << 1.0, 3.0, 2.0, 5.0, 2.0, 1.5, 0.0, 1.3, 2.5;

  const auto inverse = mat.inverse();
  const auto identity = Eigen::MatrixXd::Identity(3, 3);

  EXPECT_TRUE(identity.isApprox(mat * inverse));
}
