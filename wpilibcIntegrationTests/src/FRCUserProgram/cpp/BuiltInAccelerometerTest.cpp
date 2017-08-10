/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "BuiltInAccelerometer.h"  // NOLINT(build/include_order)

#include "Timer.h"
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
