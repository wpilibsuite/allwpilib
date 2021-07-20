// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "frc/system/NumericalIntegration.h"

// Tests that integrating dx/dt = e^x works.
TEST(NumericalIntegrationTest, Exponential) {
  Eigen::Matrix<double, 1, 1> y0;
  y0(0) = 0.0;

  Eigen::Matrix<double, 1, 1> y1 = frc::RK4(
      [](Eigen::Matrix<double, 1, 1> x) {
        Eigen::Matrix<double, 1, 1> y;
        y(0) = std::exp(x(0));
        return y;
      },
      y0, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests that integrating dx/dt = e^x works when we provide a U.
TEST(NumericalIntegrationTest, ExponentialWithU) {
  Eigen::Matrix<double, 1, 1> y0;
  y0(0) = 0.0;

  Eigen::Matrix<double, 1, 1> y1 = frc::RK4(
      [](Eigen::Matrix<double, 1, 1> x, Eigen::Matrix<double, 1, 1> u) {
        Eigen::Matrix<double, 1, 1> y;
        y(0) = std::exp(u(0) * x(0));
        return y;
      },
      y0, (Eigen::Matrix<double, 1, 1>() << 1.0).finished(), 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests that integrating dx/dt = e^x works with RKF45.
TEST(NumericalIntegrationTest, ExponentialRKF45) {
  Eigen::Matrix<double, 1, 1> y0;
  y0(0) = 0.0;

  Eigen::Matrix<double, 1, 1> y1 = frc::RKF45(
      [](Eigen::Matrix<double, 1, 1> x, Eigen::Matrix<double, 1, 1> u) {
        Eigen::Matrix<double, 1, 1> y;
        y(0) = std::exp(x(0));
        return y;
      },
      y0, (Eigen::Matrix<double, 1, 1>() << 0.0).finished(), 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests that integrating dx/dt = e^x works with RKDP
TEST(NumericalIntegrationTest, ExponentialRKDP) {
  Eigen::Matrix<double, 1, 1> y0;
  y0(0) = 0.0;

  Eigen::Matrix<double, 1, 1> y1 = frc::RKDP(
      [](Eigen::Matrix<double, 1, 1> x, Eigen::Matrix<double, 1, 1> u) {
        Eigen::Matrix<double, 1, 1> y;
        y(0) = std::exp(x(0));
        return y;
      },
      y0, (Eigen::Matrix<double, 1, 1>() << 0.0).finished(), 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}
