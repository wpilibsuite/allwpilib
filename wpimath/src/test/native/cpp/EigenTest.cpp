// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <Eigen/LU>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/linalg/EigenCore.hpp"

TEST_CASE("EigenTest Multiplication", "[wpimath]") {
  wpi::math::Matrixd<2, 2> m1{{2, 1}, {0, 1}};
  wpi::math::Matrixd<2, 2> m2{{3, 0}, {0, 2.5}};

  const auto result = m1 * m2;

  wpi::math::Matrixd<2, 2> expectedResult{{6.0, 2.5}, {0.0, 2.5}};

  CHECK(expectedResult.isApprox(result));

  wpi::math::Matrixd<2, 3> m3{{1.0, 3.0, 0.5}, {2.0, 4.3, 1.2}};
  wpi::math::Matrixd<3, 4> m4{
      {3.0, 1.5, 2.0, 4.5}, {2.3, 1.0, 1.6, 3.1}, {5.2, 2.1, 2.0, 1.0}};

  const auto result2 = m3 * m4;

  wpi::math::Matrixd<2, 4> expectedResult2{{12.5, 5.55, 7.8, 14.3},
                                           {22.13, 9.82, 13.28, 23.53}};

  CHECK(expectedResult2.isApprox(result2));
}

TEST_CASE("EigenTest Transpose", "[wpimath]") {
  Eigen::Vector3d vec{1, 2, 3};

  const auto transpose = vec.transpose();

  Eigen::RowVector3d expectedTranspose{1, 2, 3};

  CHECK(expectedTranspose.isApprox(transpose));
}

TEST_CASE("EigenTest Inverse", "[wpimath]") {
  Eigen::Matrix3d mat{{1.0, 3.0, 2.0}, {5.0, 2.0, 1.5}, {0.0, 1.3, 2.5}};

  const auto inverse = mat.inverse();
  const auto identity = Eigen::MatrixXd::Identity(3, 3);

  CHECK(identity.isApprox(mat * inverse));
}
