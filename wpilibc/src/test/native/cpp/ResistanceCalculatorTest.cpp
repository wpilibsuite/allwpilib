// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/ResistanceCalculator.h"

TEST(ResistanceCalculatorTest, EdgeCase) {
  frc::ResistanceCalculator calc;

  // Make sure it doesn't try to do a linear regression with only 1 point
  EXPECT_FALSE(calc.Calculate(1_A, 1_V).has_value());

  // Make sure points with current 0 are ignored
  EXPECT_FALSE(calc.Calculate(0_A, 1_V).has_value());
  EXPECT_FALSE(calc.Calculate(0_A, 1_V).has_value());
}

TEST(ResistanceCalculatorTest, ResistanceCalculation) {
  constexpr double tolerance = 0.5;
  frc::ResistanceCalculator calc;

  calc.Calculate(1_A, 1_V);
  EXPECT_NEAR(1.1282, calc.Calculate(40_A, 45_V).value().value(), tolerance);
  EXPECT_NEAR(1.0361, calc.Calculate(50_A, 50_V).value().value(), tolerance);
  EXPECT_NEAR(0.7832, calc.Calculate(60_A, 40_V).value().value(), tolerance);

  // R² should be too low here
  EXPECT_FALSE(calc.Calculate(100_A, 0_V).has_value());
}
