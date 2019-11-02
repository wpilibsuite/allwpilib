/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/TimedRobot.h>

#include <hal/CAN.h>
#include <hal/DIO.h>
#include <hal/DutyCycle.h>
#include <hal/HALBase.h>

#include "frc/smartdashboard/SmartDashboard.h"

HAL_DigitalHandle inputHandle;
HAL_DutyCycleHandle dutyCycleHandle;

class MyRobot : public frc::TimedRobot {
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {
    int32_t status = 0;
    inputHandle = HAL_InitializeDIOPort(HAL_GetPort(9), true, &status);
    std::cout << "DIO Status: " << status << std::endl;
    dutyCycleHandle = HAL_InitializeDutyCycle(
        inputHandle, HAL_AnalogTriggerType::HAL_Trigger_kInWindow, &status);
    std::cout << "Duty Cycle Status: " << status << std::endl;

    // float pbs = 0;
    // uint32_t boc = 0;
    // uint32_t txfc = 0;
    // uint32_t rec = 0;
    // uint32_t tec = 0;
    status = 0;
    auto start = HAL_GetFPGATime(&status);
    // HAL_CAN_GetCANStatus(&pbs, &boc, &txfc, &rec, &tec, &status);
    uint8_t data = 0;
    HAL_CAN_SendMessage(0, &data, 1, HAL_CAN_SEND_PERIOD_NO_REPEAT, &status);
    auto end = HAL_GetFPGATime(&status);
    std::cout << "Start " << start << " end " << end << std::endl;
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
  void RobotPeriodic() override {
    int32_t status = 0;
    auto freq = HAL_GetDutyCycleFrequency(dutyCycleHandle, &status);
    auto raw = HAL_GetDutyCycleOutputRaw(dutyCycleHandle, &status);
    auto percentage = HAL_GetDutyCycleOutput(dutyCycleHandle, &status);
    frc::SmartDashboard::PutNumber("Freq", freq);
    frc::SmartDashboard::PutNumber("Raw", raw);
    frc::SmartDashboard::PutNumber("Percentage", percentage);
  }
};

int main() { return frc::StartRobot<MyRobot>(); }
