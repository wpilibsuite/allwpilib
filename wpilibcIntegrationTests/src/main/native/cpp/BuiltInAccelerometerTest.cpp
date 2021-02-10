// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/BuiltInAccelerometer.h"  // NOLINT(build/include_order)

#include "frc/Timer.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kAccelerationTolerance = 0.1;
/**
 * There's not much we can automatically test with the on-board accelerometer,
 * but checking for gravity is probably good enough to tell that it's working.
 */
TEST(BuiltInAccelerometerTest, Accelerometer) {
  BuiltInAccelerometer accelerometer;

  /* The testbench sometimes shakes a little from a previous test.  Give it
          some time. */
  Wait(1.0);

  ASSERT_NEAR(0.0, accelerometer.GetX(), kAccelerationTolerance);
  ASSERT_NEAR(1.0, accelerometer.GetY(), kAccelerationTolerance);
  ASSERT_NEAR(0.0, accelerometer.GetZ(), kAccelerationTolerance);
}
