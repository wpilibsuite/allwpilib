/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

const int deviceId = 0;

TEST(CANTalonTest, QuickTest) {
	CANTalon talon(deviceId);
  talon.SetControlMode(CANSpeedController::kPercentVbus);
  talon.EnableControl();
  talon.Set(0.5);
  Wait(0.25);
  EXPECT_NEAR(talon.Get(), 0.5, 5e-3);
  talon.Set(-0.5);
  Wait(0.25);
  EXPECT_NEAR(talon.Get(), -0.5, 5e-3);

  talon.Disable();
}

TEST(CANTalonTest, SetGetPID) {
  // Tests that we can actually set and get PID values as intended.
  CANTalon talon(deviceId);
  double p = 0.05, i = 0.098, d = 1.23;
  talon.SetPID(p, i , d);
  Wait(0.1);
  EXPECT_NEAR(p, talon.GetP(), 1e-5);
  EXPECT_NEAR(i, talon.GetI(), 1e-5);
  EXPECT_NEAR(d, talon.GetD(), 1e-5);
  // Test with new values in case the talon was already set to the previous ones.
  p = 0.15;
  i = 0.198;
  d = 1.03;
  talon.SetPID(p, i , d);
  Wait(0.1);
  EXPECT_NEAR(p, talon.GetP(), 1e-5);
  EXPECT_NEAR(i, talon.GetI(), 1e-5);
  EXPECT_NEAR(d, talon.GetD(), 1e-5);
}

TEST(CANTalonTest, DISABLED_PositionModeWorks) {
  CANTalon talon(deviceId);
  double p = 1;
  double i = 0.05;
  double d = 0.01;
  talon.SetPID(p, i, d);
  talon.SetControlMode(CANSpeedController::kPosition);
  talon.Set(1000);
  talon.EnableControl();
  Wait(10.0);
  talon.Disable();
  EXPECT_NEAR(talon.Get(), 1000, 100);
}
