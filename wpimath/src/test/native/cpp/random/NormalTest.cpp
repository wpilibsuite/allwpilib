// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/random/Normal.hpp"

#include <Eigen/Core>
#include <gtest/gtest.h>

TEST(NormalTest, NormalParameterPack) {
  [[maybe_unused]]
  Eigen::Vector<double, 2> vec = wpi::math::Normal(2.0, 3.0);
}

TEST(NormalTest, NormalArray) {
  [[maybe_unused]]
  Eigen::Vector<double, 2> vec = wpi::math::Normal<2>({2.0, 3.0});
}

TEST(NormalTest, NormalDynamic) {
  [[maybe_unused]]
  Eigen::VectorXd vec = wpi::math::Normal(std::vector{2.0, 3.0});
}
