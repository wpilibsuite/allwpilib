/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"

static constexpr double accelerationTolerance = 0.1;

/**
 * There's not much we can automatically test with the on-board accelerometer,
 * but checking for gravity is probably good enough to tell that it's working.
 */
TEST(AccelerometerTest, Accelerometer) {
	BuiltInAccelerometer accelerometer;

	ASSERT_NEAR(0.0, accelerometer.GetX(), accelerationTolerance);
	ASSERT_NEAR(0.0, accelerometer.GetY(), accelerationTolerance);
	ASSERT_NEAR(1.0, accelerometer.GetZ(), accelerationTolerance);
}
