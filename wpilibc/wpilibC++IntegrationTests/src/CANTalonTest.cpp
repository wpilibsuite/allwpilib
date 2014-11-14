/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

TEST(CANTalonTest, QuickTest) {
	CANTalon talon(0);
  talon.SetControlMode(CANTalon::kPercentVbus);
  talon.EnableControl();
  talon.Set(1.0);
  Wait(0.25);
  EXPECT_GT(talon.GetOutputCurrent(), 4.0);

  talon.Set(0.0);
  talon.Disable();
}
