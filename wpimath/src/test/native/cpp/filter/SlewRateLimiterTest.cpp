// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/filter/SlewRateLimiter.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/timestamp.h"

static wpi::units::second_t now = 0_s;

class SlewRateLimiterTest {
 protected:
  SlewRateLimiterTest() {
    WPI_SetNowImpl(
        [] { return wpi::units::microsecond_t{now}.to<uint64_t>(); });
  }

  ~SlewRateLimiterTest() { WPI_SetNowImpl(nullptr); }
};

TEST_CASE_METHOD(SlewRateLimiterTest, "SlewRateLimiterTest SlewRateLimit",
                 "[wpimath]") {
  WPI_SetNowImpl([] { return wpi::units::microsecond_t{now}.to<uint64_t>(); });

  wpi::math::SlewRateLimiter<wpi::units::meters> limiter(1_mps);

  now += 1_s;

  CHECK(limiter.Calculate(2_m) < 2_m);
}

TEST_CASE_METHOD(SlewRateLimiterTest, "SlewRateLimiterTest SlewRateNoLimit",
                 "[wpimath]") {
  wpi::math::SlewRateLimiter<wpi::units::meters> limiter(1_mps);

  now += 1_s;

  CHECK(limiter.Calculate(0.5_m) == 0.5_m);
}

TEST_CASE_METHOD(SlewRateLimiterTest,
                 "SlewRateLimiterTest SlewRatePositiveNegativeLimit",
                 "[wpimath]") {
  wpi::math::SlewRateLimiter<wpi::units::meters> limiter(1_mps, -0.5_mps);

  now += 1_s;

  CHECK(limiter.Calculate(2_m) == 1_m);

  now += 1_s;

  CHECK(limiter.Calculate(0_m) == 0.5_m);
}
