// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "frc/system/NumericalIntegration.h"

// Tests that integrating dx/dt = e^x works.
TEST(NumericalIntegrationTest, Exponential) {
  frc::Vectord<1> y0{0.0};

  frc::Vectord<1> y1 = frc::RK4(
      [](const frc::Vectord<1>& x) { return frc::Vectord<1>{std::exp(x(0))}; },
      y0, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests that integrating dx/dt = e^x works when we provide a U.
TEST(NumericalIntegrationTest, ExponentialWithU) {
  frc::Vectord<1> y0{0.0};

  frc::Vectord<1> y1 = frc::RK4(
      [](const frc::Vectord<1>& x, const frc::Vectord<1>& u) {
        return frc::Vectord<1>{std::exp(u(0) * x(0))};
      },
      y0, frc::Vectord<1>{1.0}, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests that integrating dx/dt = e^x works with RKF45.
TEST(NumericalIntegrationTest, ExponentialRKF45) {
  frc::Vectord<1> y0{0.0};

  frc::Vectord<1> y1 = frc::RKF45(
      [](const frc::Vectord<1>& x, const frc::Vectord<1>& u) {
        return frc::Vectord<1>{std::exp(x(0))};
      },
      y0, frc::Vectord<1>{0.0}, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}

// Tests that integrating dx/dt = e^x works with RKDP
TEST(NumericalIntegrationTest, ExponentialRKDP) {
  frc::Vectord<1> y0{0.0};

  frc::Vectord<1> y1 = frc::RKDP(
      [](const frc::Vectord<1>& x, const frc::Vectord<1>& u) {
        return frc::Vectord<1>{std::exp(x(0))};
      },
      y0, frc::Vectord<1>{0.0}, 0.1_s);
  EXPECT_NEAR(y1(0), std::exp(0.1) - std::exp(0), 1e-3);
}
