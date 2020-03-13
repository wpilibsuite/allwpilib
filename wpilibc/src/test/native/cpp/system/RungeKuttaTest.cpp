/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <gtest/gtest.h>

#include <cmath>

#include "frc/system/RungeKutta.h"

// Tests that integrating dx/dt = e^x works.
TEST(RungeKuttaTest, Exponential) {
  Eigen::Matrix<double, 1, 1> y0;
  y0(0) = 0.0;

  Eigen::Matrix<double, 1, 1> y1 = frc::RungeKutta(
      [](Eigen::Matrix<double, 1, 1> x) {
        Eigen::Matrix<double, 1, 1> y;
        y(0) = std::exp(x(0));
        return y;
      },
      y0, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests that integrating dx/dt = e^x works when we provide a U.
TEST(RungeKuttaTest, ExponentialWithU) {
  Eigen::Matrix<double, 1, 1> y0;
  y0(0) = 0.0;

  Eigen::Matrix<double, 1, 1> y1 = frc::RungeKutta(
      [](Eigen::Matrix<double, 1, 1> x, Eigen::Matrix<double, 1, 1> u) {
        Eigen::Matrix<double, 1, 1> y;
        y(0) = std::exp(u(0) * x(0));
        return y;
      },
      y0, (Eigen::Matrix<double, 1, 1>() << 1.0).finished(), 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

namespace {
Eigen::Matrix<double, 1, 1> RungeKuttaTimeVaryingSolution(double t) {
  return (Eigen::Matrix<double, 1, 1>()
          << 12.0 * std::exp(t) / (std::pow(std::exp(t) + 1.0, 2.0)))
      .finished();
}
}  // namespace

// Tests RungeKutta with a time varying solution.
// Now, lets test RK4 with a time varying solution.  From
// http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:
//   x' = x (2 / (e^t + 1) - 1)
//
// The true (analytical) solution is:
//
// x(t) = 12 * e^t / ((e^t + 1)^2)
TEST(RungeKuttaTest, RungeKuttaTimeVarying) {
  Eigen::Matrix<double, 1, 1> y0 = RungeKuttaTimeVaryingSolution(5.0);

  Eigen::Matrix<double, 1, 1> y1 = frc::RungeKuttaTimeVarying(
      [](units::second_t t, Eigen::Matrix<double, 1, 1> x) {
        return (Eigen::Matrix<double, 1, 1>()
                << x(0) * (2.0 / (std::exp(t.to<double>()) + 1.0) - 1.0))
            .finished();
      },
      y0, 5_s, 1_s);
  EXPECT_NEAR(y1(0), RungeKuttaTimeVaryingSolution(6.0)(0), 1e-3);
}
