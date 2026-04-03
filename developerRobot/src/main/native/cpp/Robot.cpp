// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/TimedRobot.hpp"

class Robot : public wpi::TimedRobot {
 public:
  /**
   * This function is run when the robot is first started up and should be
   * used for any initialization code.
   */
  Robot() {}

  /**
   * This function is called each time the robot enters autonomous mode
   */
  void AutonomousEnter() override {}

  /**
   * This function is called periodically during autonomous
   */
  void AutonomousPeriodic() override {}

  /**
   * This function is called each time the robot enters tele-operated mode
   */
  void TeleopEnter() override {}

  /**
   * This function is called periodically during operator control
   */
  void TeleopPeriodic() override {}

  /**
   * This function is called periodically during utility mode
   */
  void UtilityPeriodic() override {}

  /**
   * This function is called periodically during all modes
   */
  void RobotPeriodic() override {}
};

int main() {
  return wpi::StartRobot<Robot>();
}
