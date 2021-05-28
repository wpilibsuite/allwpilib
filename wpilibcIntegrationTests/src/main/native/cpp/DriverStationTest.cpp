// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DriverStation.h"  // NOLINT(build/include_order)

#include <units/math.h>
#include <units/time.h>

#include "TestBench.h"
#include "frc/RobotController.h"
#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

using namespace frc;

/**
 * Test if the WaitForData function works
 */
TEST(DriverStationTest, WaitForData) {
  units::microsecond_t initialTime(RobotController::GetFPGATime());

  // 20ms waiting intervals * 50 = 1s
  for (int i = 0; i < 50; i++) {
    DriverStation::GetInstance().WaitForData();
  }

  units::microsecond_t finalTime(RobotController::GetFPGATime());

  EXPECT_NEAR_UNITS(1_s, finalTime - initialTime, 200_ms);
}
