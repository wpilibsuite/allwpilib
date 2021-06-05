// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/TimedRobot.h>

#include "frc/XboxController.h"
#include "frc/PneumaticsControlModule.h"
#include "frc/DoubleSolenoid.h"
#include "frc/Solenoid.h"

class MyRobot : public frc::TimedRobot {
  frc::PneumaticsControlModule pcm;
  frc::Solenoid solenoid{pcm, 1};
  frc::DoubleSolenoid doubleSolenoid{pcm, 2, 3};
  frc::XboxController joystick{0};

  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  void RobotInit() override {}

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
    solenoid.Set(joystick.GetAButton());
    if (joystick.GetBButton()) {
      doubleSolenoid.Set(frc::DoubleSolenoid::Value::kForward);
    } else if (joystick.GetYButton()) {
      doubleSolenoid.Set(frc::DoubleSolenoid::Value::kReverse);
    } else {
      doubleSolenoid.Set(frc::DoubleSolenoid::Value::kOff);
    }
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
