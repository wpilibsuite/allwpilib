/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Robot.h"

#include <iostream>

DriveTrain Robot::drivetrain;
Elevator Robot::elevator;
Wrist Robot::wrist;
Claw Robot::claw;
OI Robot::oi;

void Robot::RobotInit() {
  // Show what command your subsystem is running on the SmartDashboard
  frc::SmartDashboard::PutData(&drivetrain);
  frc::SmartDashboard::PutData(&elevator);
  frc::SmartDashboard::PutData(&wrist);
  frc::SmartDashboard::PutData(&claw);
}

void Robot::AutonomousInit() {
  m_autonomousCommand.Start();
  std::cout << "Starting Auto" << std::endl;
}

void Robot::AutonomousPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() {
  // This makes sure that the autonomous stops running when teleop starts
  // running. If you want the autonomous to continue until interrupted by
  // another command, remove this line or comment it out.
  m_autonomousCommand.Cancel();
  std::cout << "Starting Teleop" << std::endl;
}

void Robot::TeleopPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
