// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/util/StateSpaceUtil.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/linalg/EigenCore.hpp"

TEST_CASE("StateSpaceUtilTest CostParameterPack", "[wpimath]") {
  constexpr wpi::math::Matrixd<3, 3> mat = wpi::math::CostMatrix(1.0, 2.0, 3.0);
  CHECK_NEAR(mat(0, 0), 1.0, 1e-3);
  CHECK_NEAR(mat(0, 1), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 0), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 1), 1.0 / 4.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(2, 2), 1.0 / 9.0, 1e-3);
}

TEST_CASE("StateSpaceUtilTest CostArray", "[wpimath]") {
  constexpr wpi::math::Matrixd<3, 3> mat =
      wpi::math::CostMatrix<3>({1.0, 2.0, 3.0});
  CHECK_NEAR(mat(0, 0), 1.0, 1e-3);
  CHECK_NEAR(mat(0, 1), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 0), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 1), 1.0 / 4.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(2, 2), 1.0 / 9.0, 1e-3);
}

TEST_CASE("StateSpaceUtilTest CostDynamic", "[wpimath]") {
  Eigen::MatrixXd mat = wpi::math::CostMatrix(std::vector{1.0, 2.0, 3.0});
  CHECK_NEAR(mat(0, 0), 1.0, 1e-3);
  CHECK_NEAR(mat(0, 1), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 0), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 1), 1.0 / 4.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(2, 2), 1.0 / 9.0, 1e-3);
}

TEST_CASE("StateSpaceUtilTest CovParameterPack", "[wpimath]") {
  constexpr wpi::math::Matrixd<3, 3> mat =
      wpi::math::CovarianceMatrix(1.0, 2.0, 3.0);
  CHECK_NEAR(mat(0, 0), 1.0, 1e-3);
  CHECK_NEAR(mat(0, 1), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 0), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 1), 4.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(2, 2), 9.0, 1e-3);
}

TEST_CASE("StateSpaceUtilTest CovArray", "[wpimath]") {
  constexpr wpi::math::Matrixd<3, 3> mat =
      wpi::math::CovarianceMatrix<3>({1.0, 2.0, 3.0});
  CHECK_NEAR(mat(0, 0), 1.0, 1e-3);
  CHECK_NEAR(mat(0, 1), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 0), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 1), 4.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(2, 2), 9.0, 1e-3);
}

TEST_CASE("StateSpaceUtilTest CovDynamic", "[wpimath]") {
  Eigen::MatrixXd mat = wpi::math::CovarianceMatrix(std::vector{1.0, 2.0, 3.0});
  CHECK_NEAR(mat(0, 0), 1.0, 1e-3);
  CHECK_NEAR(mat(0, 1), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 0), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 1), 4.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(0, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(1, 2), 0.0, 1e-3);
  CHECK_NEAR(mat(2, 2), 9.0, 1e-3);
}

TEST_CASE("StateSpaceUtilTest DesaturateInputVector", "[wpimath]") {
  constexpr Eigen::Vector2d vec1{{10.0, 12.0}};
  CHECK(wpi::math::DesaturateInputVector<2>(vec1, 12.0) == vec1);
  CHECK(wpi::math::DesaturateInputVector<2>(vec1, 10.0) ==
        (Eigen::Vector2d{{25.0 / 3.0}, {10.0}}));

  constexpr Eigen::Vector2d vec2{{10.0, -12.0}};
  CHECK(wpi::math::DesaturateInputVector<2>(vec2, 12.0) == vec2);
  CHECK(wpi::math::DesaturateInputVector<2>(vec2, 10.0) ==
        (Eigen::Vector2d{{25.0 / 3.0}, {-10.0}}));

  constexpr Eigen::Vector2d vec3{{0.0, 0.0}};
  CHECK(wpi::math::DesaturateInputVector<2>(vec3, 12.0) == vec3);
}
