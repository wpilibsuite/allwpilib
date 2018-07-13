/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
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

int main() { return frc::StartRobot<Robot>(); }
