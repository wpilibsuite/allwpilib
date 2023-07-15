// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Eigen/LU"
#include "frc/EigenCore.h"
#include "gtest/gtest.h"

TEST(EigenTest, Multiplication) {
  frc::Matrixd<2, 2> m1{{2, 1}, {0, 1}};
  frc::Matrixd<2, 2> m2{{3, 0}, {0, 2.5}};

  const auto result = m1 * m2;

  frc::Matrixd<2, 2> expectedResult{{6.0, 2.5}, {0.0, 2.5}};

  EXPECT_TRUE(expectedResult.isApprox(result));

  frc::Matrixd<2, 3> m3{{1.0, 3.0, 0.5}, {2.0, 4.3, 1.2}};
  frc::Matrixd<3, 4> m4{
      {3.0, 1.5, 2.0, 4.5}, {2.3, 1.0, 1.6, 3.1}, {5.2, 2.1, 2.0, 1.0}};

  const auto result2 = m3 * m4;

  frc::Matrixd<2, 4> expectedResult2{{12.5, 5.55, 7.8, 14.3},
                                     {22.13, 9.82, 13.28, 23.53}};

  EXPECT_TRUE(expectedResult2.isApprox(result2));
}

TEST(EigenTest, Transpose) {
  frc::Vectord<3> vec{1, 2, 3};

  const auto transpose = vec.transpose();

  Eigen::RowVector<double, 3> expectedTranspose{1, 2, 3};

  EXPECT_TRUE(expectedTranspose.isApprox(transpose));
}

TEST(EigenTest, Inverse) {
  frc::Matrixd<3, 3> mat{{1.0, 3.0, 2.0}, {5.0, 2.0, 1.5}, {0.0, 1.3, 2.5}};

  const auto inverse = mat.inverse();
  const auto identity = Eigen::MatrixXd::Identity(3, 3);

  EXPECT_TRUE(identity.isApprox(mat * inverse));
}
