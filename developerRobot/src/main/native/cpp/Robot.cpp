// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/Gamepad.hpp"
#include "wpi/framework/TimedRobot.hpp"

class Robot : public wpi::TimedRobot {
 public:
  wpi::Gamepad m_gamepad{0};
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  Robot() {}

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
    double leftTrigger = m_gamepad.GetLeftTriggerAxis();
    double rightTrigger = m_gamepad.GetRightTriggerAxis();

    m_gamepad.SetRumble(wpi::GenericHID::kLeftRumble, leftTrigger);
    m_gamepad.SetRumble(wpi::GenericHID::kRightRumble, rightTrigger);

    if (m_gamepad.GetSouthFaceButton()) {
      m_gamepad.SetLeds(0, 255, 0);  // Green
    } else if (m_gamepad.GetNorthFaceButton()) {
      m_gamepad.SetLeds(255, 0, 0);  // Red
    } else if (m_gamepad.GetEastFaceButton()) {
      m_gamepad.SetLeds(0, 0, 255);  // Blue
    } else if (m_gamepad.GetWestFaceButton()) {
      m_gamepad.SetLeds(255, 255, 0);  // Yellow
    } else {
      m_gamepad.SetLeds(0, 0, 0);  // Off
    }
  }
};

int main() {
  return wpi::StartRobot<Robot>();
}
