// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/DriverStation.h"  // NOLINT(build/include_order)

#include <hal/DriverStation.h>
#include <units/math.h>
#include <units/time.h>

#include "TestBench.h"
#include "frc/RobotController.h"
#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

/**
 * Test if the WaitForData function works
 */
TEST(DriverStationTest, WaitForData) {
  units::microsecond_t initialTime(frc::RobotController::GetFPGATime());

  wpi::Event waitEvent{true};
  HAL_ProvideNewDataEventHandle(waitEvent.GetHandle());

  // 20ms waiting intervals * 50 = 1s
  for (int i = 0; i < 50; i++) {
    wpi::WaitForObject(waitEvent.GetHandle());
  }

  HAL_RemoveNewDataEventHandle(waitEvent.GetHandle());

  units::microsecond_t finalTime(frc::RobotController::GetFPGATime());

  EXPECT_NEAR_UNITS(1_s, finalTime - initialTime, 200_ms);
}
