// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Timer.h"  // NOLINT(build/include_order)

#include <units/math.h>

#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

TEST(TimerTest, Wait) {
  auto initialTime = frc::Timer::GetFPGATimestamp();

  frc::Wait(500_ms);

  auto finalTime = frc::Timer::GetFPGATimestamp();

  EXPECT_NEAR_UNITS(500_ms, finalTime - initialTime, 1_ms);
}
