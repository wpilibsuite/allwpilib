// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/TimedRobot.h>
#include <frc/PneumaticHub.h>
#include <frc/AnalogOutput.h>
#include <frc/smartdashboard/SmartDashboard.h>

class MyRobot : public frc::TimedRobot {
  frc::AnalogOutput ao{0};
  frc::PneumaticHub hub{1};

  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {
    frc::SmartDashboard::PutNumber("AOut", 0);
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
    ao.SetVoltage(frc::SmartDashboard::GetNumber("AOut", 0));
    frc::SmartDashboard::PutNumber("Pressure", hub.GetAnalogVoltage(0).value());
  }
};

int main() {
  return frc::StartRobot<MyRobot>();
}
