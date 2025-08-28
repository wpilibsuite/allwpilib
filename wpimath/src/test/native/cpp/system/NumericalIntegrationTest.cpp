// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "wpimath/EigenCore.h"
#include "wpimath/system/NumericalIntegration.h"

// Test that integrating dx/dt = eˣ works
TEST(NumericalIntegrationTest, Exponential) {
  wpimath::Vectord<1> y0{0.0};

  wpimath::Vectord<1> y1 = wpimath::RK4(
      [](const wpimath::Vectord<1>& x) {
        return wpimath::Vectord<1>{std::exp(x(0))};
      },
      y0, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Test that integrating dx/dt = eˣ works when we provide a u
TEST(NumericalIntegrationTest, ExponentialWithU) {
  wpimath::Vectord<1> y0{0.0};

  wpimath::Vectord<1> y1 = wpimath::RK4(
      [](const wpimath::Vectord<1>& x, const wpimath::Vectord<1>& u) {
        return wpimath::Vectord<1>{std::exp(u(0) * x(0))};
      },
      y0, wpimath::Vectord<1>{1.0}, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests RK4 with a time varying solution. From
// http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:
//
//   dx/dt = x (2 / (eᵗ + 1) - 1)
//
// The true (analytical) solution is:
//
//   x(t) = 12eᵗ/(eᵗ + 1)²
TEST(NumericalIntegrationTest, RK4TimeVarying) {
  wpimath::Vectord<1> y0{12.0 * std::exp(5.0) /
                         std::pow(std::exp(5.0) + 1.0, 2.0)};

  wpimath::Vectord<1> y1 = wpimath::RK4(
      [](units::second_t t, const wpimath::Vectord<1>& x) {
        return wpimath::Vectord<1>{x(0) *
                                   (2.0 / (std::exp(t.value()) + 1.0) - 1.0)};
      },
      5_s, y0, 1_s);
  EXPECT_NEAR(y1(0), 12.0 * std::exp(6.0) / std::pow(std::exp(6.0) + 1.0, 2.0),
              1e-3);
}

// Tests that integrating dx/dt = 0 works with RKDP
TEST(NumericalIntegrationTest, ZeroRKDP) {
  wpimath::Vectord<1> y1 = wpimath::RKDP(
      [](const wpimath::Vectord<1>& x, const wpimath::Vectord<1>& u) {
        return wpimath::Vectord<1>::Zero();
      },
      wpimath::Vectord<1>{0.0}, wpimath::Vectord<1>{0.0}, 0.1_s);
  EXPECT_NEAR(y1(0), 0.0, 1e-3);
}

// Tests that integrating dx/dt = eˣ works with RKDP
TEST(NumericalIntegrationTest, ExponentialRKDP) {
  wpimath::Vectord<1> y0{0.0};

  wpimath::Vectord<1> y1 = wpimath::RKDP(
      [](const wpimath::Vectord<1>& x, const wpimath::Vectord<1>& u) {
        return wpimath::Vectord<1>{std::exp(x(0))};
      },
      y0, wpimath::Vectord<1>{0.0}, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests RKDP with a time varying solution. From
// http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:
//
//   dx/dt = x(2/(eᵗ + 1) - 1)
//
// The true (analytical) solution is:
//
//   x(t) = 12eᵗ/(eᵗ + 1)²
TEST(NumericalIntegrationTest, RKDPTimeVarying) {
  wpimath::Vectord<1> y0{12.0 * std::exp(5.0) /
                         std::pow(std::exp(5.0) + 1.0, 2.0)};

  wpimath::Vectord<1> y1 = wpimath::RKDP(
      [](units::second_t t, const wpimath::Vectord<1>& x) {
        return wpimath::Vectord<1>{x(0) *
                                   (2.0 / (std::exp(t.value()) + 1.0) - 1.0)};
      },
      5_s, y0, 1_s, 1e-12);
  EXPECT_NEAR(y1(0), 12.0 * std::exp(6.0) / std::pow(std::exp(6.0) + 1.0, 2.0),
              1e-3);
}
