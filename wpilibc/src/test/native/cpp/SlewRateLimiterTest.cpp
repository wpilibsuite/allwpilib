// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <thread>

#include <units/length.h>
#include <units/time.h>
#include <units/velocity.h>

#include "frc/SlewRateLimiter.h"
#include "frc/simulation/SimHooks.h"
#include "gtest/gtest.h"

TEST(SlewRateLimiterTest, SlewRateLimitTest) {
  frc::SlewRateLimiter<units::meters> limiter(1_mps);

  frc::sim::StepTiming(1.0_s);

  EXPECT_TRUE(limiter.Calculate(2_m) < 2_m);
}

TEST(SlewRateLimiterTest, SlewRateNoLimitTest) {
  frc::SlewRateLimiter<units::meters> limiter(1_mps);

  frc::sim::StepTiming(1.0_s);

  EXPECT_EQ(limiter.Calculate(0.5_m), 0.5_m);
}
