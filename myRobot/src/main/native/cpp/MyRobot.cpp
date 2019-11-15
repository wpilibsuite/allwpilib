/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/



// #include <frc/TimedRobot.h>

// #include "hal/AddressableLED.h"
// #include "hal/DIO.h"
// #include "hal/HALBase.h"

// #include "hal/ChipObject.h"
// #include "FRC_NetworkCommunication/LoadOut.h"

// class MyRobot : public frc::TimedRobot {

//   HAL_DigitalHandle output;
//   HAL_AddressableLEDHandle ledHandle;

//   /**
//    * This function is run when the robot is first started up and should be
//    * used for any initialization code.
//    */
//   void RobotInit() override {

//     int32_t status = 0;
//     output = HAL_InitializeDIOPort(HAL_GetPort(0), false, &status);
//     std::cout << status << std::endl;
//     ledHandle = HAL_InitializeAddressableLED(output, &status);
//     std::cout << status << std::endl;

//     HAL_WriteAddressableLEDStringLength(ledHandle, 400, &status);

//     //HAL_SetAddressableLEDTiming(ledHandle, 1000, 3000, 3000, 1000, 100, &status);
//     HAL_SetAddressableLEDTiming(ledHandle, 350, 800, 700, 600, 100, &status);

//     HAL_WriteAddressableLEDOnce(ledHandle, &status);

//     //std::this_thread::sleep_for(std::chrono::milliseconds(100));

//     //HAL_StopAddressableLEDWrite(ledHandle, &status);

//     std::cout << status << std::endl;


//   }

//   /**
//    * This function is run once each time the robot enters autonomous mode
//    */
//   void AutonomousInit() override {}

//   /**
//    * This function is called periodically during autonomous
//    */
//   void AutonomousPeriodic() override {}

//   /**
//    * This function is called once each time the robot enters tele-operated mode
//    */
//   void TeleopInit() override {}

//   /**
//    * This function is called periodically during operator control
//    */
//   void TeleopPeriodic() override {}

//   /**
//    * This function is called periodically during test mode
//    */
//   void TestPeriodic() override {}

//   /**
//    * This function is called periodically during all modes
//    */
//   void RobotPeriodic() override {}
// };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tGlobal.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tSysWatchdog.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/tLED.h"
#include "FRC_FPGA_ChipObject/nRoboRIO_FPGANamespace/nInterfaceGlobals.h"
#include "FRC_NetworkCommunication/LoadOut.h"

#pragma GCC diagnostic pop

#include <iostream>

