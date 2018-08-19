/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Robot.h"

#include <iostream>

#include <frc/Timer.h>
#include <frc/smartdashboard/SmartDashboard.h>

Robot::Robot() {
  // Note SmartDashboard is not initialized here, wait until RobotInit() to make
  // SmartDashboard calls
  m_robotDrive.SetExpiration(0.1);
}

void Robot::RobotInit() {
  m_chooser.SetDefaultOption(kAutoNameDefault, kAutoNameDefault);
  m_chooser.AddOption(kAutoNameCustom, kAutoNameCustom);
  frc::SmartDashboard::PutData("Auto Modes", &m_chooser);
}

/**
 * This autonomous (along with the chooser code above) shows how to select
 * between different autonomous modes using the dashboard. The sendable chooser
 * code works with the Java SmartDashboard. If you prefer the LabVIEW Dashboard,
 * remove all of the chooser code and uncomment the GetString line to get the
 * auto name from the text box below the Gyro.
 *
 * You can add additional auto modes by adding additional comparisons to the
 * if-else structure below with additional strings. If using the SendableChooser
 * make sure to add them to the chooser code above as well.
 */
void Robot::Autonomous() {
  std::string autoSelected = m_chooser.GetSelected();
  // std::string autoSelected = frc::SmartDashboard::GetString(
  // "Auto Selector", kAutoNameDefault);
  std::cout << "Auto selected: " << autoSelected << std::endl;

  // MotorSafety improves safety when motors are updated in loops but is
  // disabled here because motor updates are not looped in this autonomous mode.
  m_robotDrive.SetSafetyEnabled(false);

  if (autoSelected == kAutoNameCustom) {
    // Custom Auto goes here
    std::cout << "Running custom Autonomous" << std::endl;

    // Spin at half speed for two seconds
    m_robotDrive.ArcadeDrive(0.0, 0.5);
    frc::Wait(2.0);

    // Stop robot
    m_robotDrive.ArcadeDrive(0.0, 0.0);
  } else {
    // Default Auto goes here
    std::cout << "Running default Autonomous" << std::endl;

    // Drive forwards at half speed for two seconds
    m_robotDrive.ArcadeDrive(-0.5, 0.0);
    frc::Wait(2.0);

    // Stop robot
    m_robotDrive.ArcadeDrive(0.0, 0.0);
  }
}

/**
 * Runs the motors with arcade steering.
 */
void Robot::OperatorControl() {
  m_robotDrive.SetSafetyEnabled(true);
  while (IsOperatorControl() && IsEnabled()) {
    // Drive with arcade style (use right stick)
    m_robotDrive.ArcadeDrive(-m_stick.GetY(), m_stick.GetX());

    // The motors will be updated every 5ms
    frc::Wait(0.005);
  }
}

/**
 * Runs during test mode
 */
void Robot::Test() {}

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif
