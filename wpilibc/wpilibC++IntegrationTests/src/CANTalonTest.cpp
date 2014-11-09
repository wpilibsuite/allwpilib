/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "WPILib.h"
#include "gtest/gtest.h"
#include "TestBench.h"

TEST(CANTalonTest, QuickTest) {
	CANTalon talon(10);

	for(;;) {
		std::cout << "Firmware version: " << talon.GetFirmwareVersion() << std::endl;
		Wait(0.25);
	}
}
