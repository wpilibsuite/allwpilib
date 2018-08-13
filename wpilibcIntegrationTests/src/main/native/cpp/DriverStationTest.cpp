/*----------------------------------------------------------------------------*/
/* Copyright (c) 2014-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DriverStation.h"  // NOLINT(build/include_order)

#include "TestBench.h"
#include "frc/RobotController.h"
#include "gtest/gtest.h"

using namespace frc;

constexpr double TIMER_TOLERANCE = 0.2;
constexpr int64_t TIMER_RUNTIME = 1000000;  // 1 second

class DriverStationTest : public testing::Test {};

/**
 * Test if the WaitForData function works
 */
TEST_F(DriverStationTest, WaitForData) {
  uint64_t initialTime = RobotController::GetFPGATime();

  for (int i = 0; i < 50; i++) {
    DriverStation::GetInstance().WaitForData();
  }

  uint64_t finalTime = RobotController::GetFPGATime();

  EXPECT_NEAR(TIMER_RUNTIME, finalTime - initialTime,
              TIMER_TOLERANCE * TIMER_RUNTIME);
}
