// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/ResistanceCalculator.h"

TEST(ResistanceCalculatorTest, EdgeCase) {
  frc::ResistanceCalculator calc;

  // Make sure it doesn't try to do a linear regression with only 1 point
  EXPECT_TRUE(std::isnan(calc.Calculate(1_A, 1_V).value()));

  // Make sure points with current 0 are ignored
  EXPECT_TRUE(std::isnan(calc.Calculate(0_A, 1_V).value()));
  EXPECT_TRUE(std::isnan(calc.Calculate(0_A, 1_V).value()));
}

TEST(ResistanceCalculatorTest, ResistanceCalculation) {
  constexpr double tolerance = 0.5;
  frc::ResistanceCalculator calc;

  calc.Calculate(1_A, 1_V);
  EXPECT_NEAR(1.1282, calc.Calculate(40_A, 45_V).value(), tolerance);
  EXPECT_NEAR(1.0361, calc.Calculate(50_A, 50_V).value(), tolerance);
  EXPECT_NEAR(0.7832, calc.Calculate(60_A, 40_V).value(), tolerance);

  // R² should be too low here
  EXPECT_TRUE(std::isnan(calc.Calculate(100_A, 0_V).value()));
}
