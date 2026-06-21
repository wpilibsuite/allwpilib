// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/random/Normal.hpp"

#include <Eigen/Core>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("NormalTest NormalParameterPack", "[wpimath]") {
  [[maybe_unused]]
  Eigen::Vector<double, 2> vec = wpi::math::Normal(2.0, 3.0);
}

TEST_CASE("NormalTest NormalArray", "[wpimath]") {
  [[maybe_unused]]
  Eigen::Vector<double, 2> vec = wpi::math::Normal<2>({2.0, 3.0});
}

TEST_CASE("NormalTest NormalDynamic", "[wpimath]") {
  [[maybe_unused]]
  Eigen::VectorXd vec = wpi::math::Normal(std::vector{2.0, 3.0});
}
