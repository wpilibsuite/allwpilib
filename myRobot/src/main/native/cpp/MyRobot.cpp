// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/TimedRobot.h>
#include "hal/CTREPCM.h"
#include "frc/Solenoid.h"

class MyRobot : public frc::TimedRobot {
  //HAL_CTREPCMHandle handle;
  frc::Solenoid* solenoid;
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {
    //int32_t status = 0;

    solenoid = new frc::Solenoid{0};
    //handle = HAL_InitializeCTREPCM(0, nullptr, &status);
    //printf("Status %d\n", status);
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
  void TeleopPeriodic() override {
    //int32_t status = 0;
    solenoid->Set(true);
    //HAL_SetCTREPCMSolenoids(handle, 0xFF, 1, &status);
    //printf("Status %d\n", status);
  }

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
  return frc::StartRobot<MyRobot>();
}
