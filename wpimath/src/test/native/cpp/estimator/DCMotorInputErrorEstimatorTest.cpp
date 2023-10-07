// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/estimator/DCMotorInputErrorEstimator.h"
#include "units/length.h"

TEST(DCMotorInputErrorEstimatorTest, PositiveBias) {
  frc::DCMotorInputErrorEstimator<units::meter> estimator{
      1_V / 1_mps, 0.5_V / 1_mps_sq, 5_ms};
  estimator.Reset(1_m, 0_mps);
  EXPECT_DOUBLE_EQ(5.637486730707648, estimator.Calculate(2_V, 2_m).value());
}
