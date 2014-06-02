/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "WPILib.h"

int main(int argc, char *argv[]) {
	testing::InitGoogleTest(&argc, argv);
	
	if(!HALInitialize()) {
		std::cerr << "FATAL ERROR: HAL could not be initialized" << std::endl;
		return -1;
	}
	
	/* This sets up the network communications library to enable the driver
	  station. After starting network coms, it will loop until the driver
	  station returns that the robot is enabled, to ensure that tests will be
	  able to run on the hardware. */  
	HALNetworkCommunicationObserveUserProgramStarting();
	LiveWindow::GetInstance()->SetEnabled(false);
	
	while(!DriverStation::GetInstance()->IsEnabled()) {
		Wait(0.1);
		std::cout << "Waiting for enable" << std::endl;
	}
	
	std::cout << "Running!" << std::endl;
	    
	return RUN_ALL_TESTS();
}
