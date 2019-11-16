/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/



#include <frc/TimedRobot.h>

#include "hal/AddressableLED.h"
#include "hal/DIO.h"
#include "hal/PWM.h"
#include "hal/HALBase.h"

#include "hal/ChipObject.h"
#include "FRC_NetworkCommunication/LoadOut.h"

class MyRobot : public frc::TimedRobot {

  HAL_DigitalHandle output;
  HAL_AddressableLEDHandle ledHandle;

  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {

    int32_t status = 0;
    output = HAL_InitializePWMPort(HAL_GetPort(0), &status);
    std::cout << status << std::endl;
    ledHandle = HAL_InitializeAddressableLED(output, &status);
    std::cout << status << std::endl;

    HAL_AddressableLEDData buf[12];
    std::memset(buf, 0, sizeof(buf));
    for (int i = 0; i < 12; i += 3) {
      buf[i].g = 0x96;
      buf[i + 1].b = 0x96;
      buf[i + 2].r = 0x96;
    }

    HAL_WriteAddressableLEDData(ledHandle, buf, 12, &status);

    std::cout << status << std::endl;

    //HAL_SetAddressableLEDTiming(ledHandle, 1000, 3000, 3000, 1000, 100, &status);
    //HAL_SetAddressableLEDTiming(ledHandle, 350, 800, 700, 600, 500, &status);

    std::cout << status << std::endl;

    HAL_StartAddressableLEDWrite(ledHandle, &status);



    //std::this_thread::sleep_for(std::chrono::milliseconds(100));

    //HAL_StopAddressableLEDWrite(ledHandle, &status);

    std::cout << status << std::endl;

    //HAL_ExitMain();


  }

  /**
   * This function is run once each time the robot enters autonomous mode
   */
  void AutonomousInit() override {}

  /**
   * This function is called periodically during autonomous
   */
  void AutonomousPeriodic() override {}

  /**
   * This function is called once each time the robot enters tele-operated mode
   */
  void TeleopInit() override {}

  /**
   * This function is called periodically during operator control
   */
  void TeleopPeriodic() override {}

  /**
   * This function is called periodically during test mode
   */
  void TestPeriodic() override {}

  /**
   * This function is called periodically during all modes
   */
  void RobotPeriodic() override {}
};


int main() {
  frc::StartRobot<MyRobot>();
}
