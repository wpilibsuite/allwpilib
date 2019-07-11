/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Robot.h"

#include <iostream>

#include <frc/commands/Scheduler.h>
#include <frc/livewindow/LiveWindow.h>
#include <frc/smartdashboard/SmartDashboard.h>

DriveTrain Robot::drivetrain;
Pivot Robot::pivot;
Collector Robot::collector;
Shooter Robot::shooter;
Pneumatics Robot::pneumatics;
OI Robot::oi;

void Robot::RobotInit() {
  // Show what command your subsystem is running on the SmartDashboard
  frc::SmartDashboard::PutData(&drivetrain);
  frc::SmartDashboard::PutData(&pivot);
  frc::SmartDashboard::PutData(&collector);
  frc::SmartDashboard::PutData(&shooter);
  frc::SmartDashboard::PutData(&pneumatics);

  // instantiate the command used for the autonomous period
  m_autoChooser.SetDefaultOption("Drive and Shoot", &m_driveAndShootAuto);
  m_autoChooser.AddOption("Drive Forward", &m_driveForwardAuto);
  frc::SmartDashboard::PutData("Auto Mode", &m_autoChooser);

  pneumatics.Start();  // Pressurize the pneumatics.
}

void Robot::AutonomousInit() {
  m_autonomousCommand = m_autoChooser.GetSelected();
  m_autonomousCommand->Start();
}

void Robot::AutonomousPeriodic() {
  frc::Scheduler::GetInstance()->Run();
  Log();
}

void Robot::TeleopInit() {
  // This makes sure that the autonomous stops running when
  // teleop starts running. If you want the autonomous to
  // continue until interrupted by another command, remove
  // this line or comment it out.
  if (m_autonomousCommand != nullptr) {
    m_autonomousCommand->Cancel();
  }
  std::cout << "Starting Teleop" << std::endl;
}

void Robot::TeleopPeriodic() {
  frc::Scheduler::GetInstance()->Run();
  Log();
}

void Robot::TestPeriodic() {}

void Robot::DisabledInit() { shooter.Unlatch(); }

void Robot::DisabledPeriodic() { Log(); }

/**
 * Log interesting values to the SmartDashboard.
 */
void Robot::Log() {
  Robot::pneumatics.WritePressure();
  frc::SmartDashboard::PutNumber("Pivot Pot Value", pivot.GetAngle());
  frc::SmartDashboard::PutNumber("Left Distance",
                                 drivetrain.GetLeftEncoder().GetDistance());
  frc::SmartDashboard::PutNumber("Right Distance",
                                 drivetrain.GetRightEncoder().GetDistance());
}

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
