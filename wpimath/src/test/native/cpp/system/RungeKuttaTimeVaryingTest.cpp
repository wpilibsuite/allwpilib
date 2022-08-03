// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "frc/system/RungeKuttaTimeVarying.h"

namespace {
frc::Vectord<1> RungeKuttaTimeVaryingSolution(double t) {
  return frc::Vectord<1>{12.0 * std::exp(t) / std::pow(std::exp(t) + 1.0, 2.0)};
}
}  // namespace

// Tests RK4 with a time varying solution. From
// http://www2.hawaii.edu/~jmcfatri/math407/RungeKuttaTest.html:
//   x' = x (2/(eᵗ + 1) - 1)
//
// The true (analytical) solution is:
//
// x(t) = 12eᵗ/((eᵗ + 1)²)
TEST(RungeKuttaTimeVaryingTest, RungeKuttaTimeVarying) {
  frc::Vectord<1> y0 = RungeKuttaTimeVaryingSolution(5.0);

  frc::Vectord<1> y1 = frc::RungeKuttaTimeVarying(
      [](units::second_t t, const frc::Vectord<1>& x) {
        return frc::Vectord<1>{x(0) *
                               (2.0 / (std::exp(t.value()) + 1.0) - 1.0)};
      },
      5_s, y0, 1_s);
  EXPECT_NEAR(y1(0), RungeKuttaTimeVaryingSolution(6.0)(0), 1e-3);
}
