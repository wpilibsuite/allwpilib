// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/timestamp.h>

#include "units/length.h"
#include "units/time.h"
#include "units/velocity.h"
#include "wpi/math/filter/SlewRateLimiter.h"

static units::second_t now = 0_s;

class SlewRateLimiterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    WPI_SetNowImpl([] { return units::microsecond_t{now}.to<uint64_t>(); });
  }

  void TearDown() override { WPI_SetNowImpl(nullptr); }
};

TEST_F(SlewRateLimiterTest, SlewRateLimit) {
  WPI_SetNowImpl([] { return units::microsecond_t{now}.to<uint64_t>(); });

  wpi::math::SlewRateLimiter<units::meters> limiter(1_mps);

  now += 1_s;

  EXPECT_LT(limiter.Calculate(2_m), 2_m);
}

TEST_F(SlewRateLimiterTest, SlewRateNoLimit) {
  wpi::math::SlewRateLimiter<units::meters> limiter(1_mps);

  now += 1_s;

  EXPECT_EQ(limiter.Calculate(0.5_m), 0.5_m);
}

TEST_F(SlewRateLimiterTest, SlewRatePositiveNegativeLimit) {
  wpi::math::SlewRateLimiter<units::meters> limiter(1_mps, -0.5_mps);

  now += 1_s;

  EXPECT_EQ(limiter.Calculate(2_m), 1_m);

  now += 1_s;

  EXPECT_EQ(limiter.Calculate(0_m), 0.5_m);
}
