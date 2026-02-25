// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

Robot::Robot() {
  wpi::util::SendableRegistry::AddChild(&m_drive, &m_leftMotor);
  wpi::util::SendableRegistry::AddChild(&m_drive, &m_rightMotor);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightMotor.SetInverted(true);
}

/**
 * This function is called every 20 ms, no matter the mode. Use
 * this for items like diagnostics that you want ran during disabled,
 * autonomous, teleoperated and test.
 *
 * <p> This runs after the mode specific periodic functions, but before
 * LiveWindow and SmartDashboard integrated updating.
 */
void Robot::RobotPeriodic() {}

// This function is only once each time Autonomous is enabled
void Robot::AutonomousInit() {
  m_timer.Restart();
}

// This function is called periodically during autonomous mode
void Robot::AutonomousPeriodic() {
  // Drive for 2 seconds
  if (m_timer.Get() < 2_s) {
    // Drive forwards half speed, make sure to turn input squaring off
    m_drive.ArcadeDrive(0.5, 0.0, false);
  } else {
    // Stop robot
    m_drive.ArcadeDrive(0.0, 0.0, false);
  }
}

// This function is only once each time telop is enabled
void Robot::TeleopInit() {}

// This function is called periodically during teleop mode
void Robot::TeleopPeriodic() {
  m_drive.ArcadeDrive(-m_controller.GetRawAxis(2), -m_controller.GetRawAxis(1));
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
