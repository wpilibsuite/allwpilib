/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <frc/TimedRobot.h>
#include <frc/DutyCycleEncoder.h>
#include <frc/DutyCycle.h>
#include <frc/DigitalInput.h>
#include <frc/DigitalOutput.h>
#include <frc/smartdashboard/SmartDashboard.h>


class MyRobot : public frc::TimedRobot {
  frc::DigitalInput di{0};
  frc::DutyCycle dc{di};
  frc::DutyCycleEncoder dce{dc};
  frc::DigitalOutput digO{1};
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {
    digO.SetPWMRate(500);
    digO.EnablePWM(0.5);
    digO.UpdateDutyCycle(0.75);

    frc::SmartDashboard::PutNumber("DC", 0.75);

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
    double val = frc::SmartDashboard::GetNumber("DC", 0.5);
    if (val < 0) val = 0;
    if (val > 1) val = 1;
    digO.UpdateDutyCycle(val);
  }
};

int main() { return frc::StartRobot<MyRobot>(); }
