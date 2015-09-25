/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include <CANTalon.h>
#include <Timer.h>
#include "gtest/gtest.h"
#include "TestBench.h"

const int deviceId = 0;

TEST(CANTalonTest, QuickTest) {
  double throttle = 0.1;
  CANTalon talon(deviceId);
  talon.SetControlMode(CANSpeedController::kPercentVbus);
  talon.EnableControl();
  talon.Set(throttle);
  Wait(0.25);
  EXPECT_NEAR(talon.Get(), throttle, 5e-3);
  talon.Set(-throttle);
  Wait(0.25);
  EXPECT_NEAR(talon.Get(), -throttle, 5e-3);

  talon.Disable();
  Wait(0.1);
  EXPECT_FLOAT_EQ(talon.Get(), 0.0);
}

TEST(CANTalonTest, SetGetPID) {
  // Tests that we can actually set and get PID values as intended.
  CANTalon talon(deviceId);
  double p = 0.05, i = 0.098, d = 1.23;
  talon.SetPID(p, i, d);
  // Wait(0.03);
  EXPECT_NEAR(p, talon.GetP(), 1e-5);
  EXPECT_NEAR(i, talon.GetI(), 1e-5);
  EXPECT_NEAR(d, talon.GetD(), 1e-5);
  // Test with new values in case the talon was already set to the previous
  // ones.
  p = 0.15;
  i = 0.198;
  d = 1.03;
  talon.SetPID(p, i, d);
  // Wait(0.03);
  EXPECT_NEAR(p, talon.GetP(), 1e-5);
  EXPECT_NEAR(i, talon.GetI(), 1e-5);
  EXPECT_NEAR(d, talon.GetD(), 1e-5);
}

TEST(CANTalonTest, DISABLED_PositionModeWorks) {
  CANTalon talon(deviceId);
  talon.SetFeedbackDevice(CANTalon::AnalogPot);
  talon.SetControlMode(CANSpeedController::kPosition);
  Wait(0.1);
  double p = 2;
  double i = 0.00;
  double d = 0.00;
  Wait(0.2);
  talon.SetControlMode(CANSpeedController::kPosition);
  talon.SetFeedbackDevice(CANTalon::AnalogPot);
  talon.SetPID(p, i, d);
  Wait(0.2);
  talon.Set(100);
  Wait(100);
  talon.Disable();
  EXPECT_NEAR(talon.Get(), 500, 1000);
}
