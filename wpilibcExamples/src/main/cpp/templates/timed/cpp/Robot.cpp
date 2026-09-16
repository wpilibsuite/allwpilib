// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/tunables/Tunables.hpp"
#include "wpi/util/print.hpp"

Robot::Robot() {
  chooser.AddDefault(AUTO_NAME_DEFAULT, AUTO_NAME_DEFAULT);
  chooser.Add(AUTO_NAME_CUSTOM, AUTO_NAME_CUSTOM);
  wpi::tunables::Publish("Auto Modes", chooser);
}

/**
 * This function is called every 20 ms, no matter the mode. Use
 * this for items like diagnostics that you want ran during disabled,
 * autonomous, teleoperated and utility.
 *
 * <p> This runs after the mode specific periodic functions, but before
 * LiveWindow and SmartDashboard integrated updating.
 */
void Robot::RobotPeriodic() {}

/**
 * This autonomous (along with the chooser code above) shows how to select
 * between different autonomous modes using the dashboard. The sendable chooser
 * code works with the Java SmartDashboard. If you prefer the LabVIEW Dashboard,
 * remove all of the chooser code and uncomment the GetString line to get the
 * auto name from the text box below the Gyro.
 *
 * You can add additional auto modes by adding additional comparisons to the
 * if-else structure below with additional strings. If using
 * wpi::tunables::Selectable make sure to add them to the chooser code above as
 * well.
 */
void Robot::AutonomousInit() {
  autoSelected = chooser.GetSelected();
  // autoSelected = SmartDashboard::GetString("Auto Selector",
  //     AUTO_NAME_DEFAULT);
  wpi::util::print("Auto selected: {}\n", autoSelected);

  if (autoSelected == AUTO_NAME_CUSTOM) {
    // Custom Auto goes here
  } else {
    // Default Auto goes here
  }
}

void Robot::AutonomousPeriodic() {
  if (autoSelected == AUTO_NAME_CUSTOM) {
    // Custom Auto goes here
  } else {
    // Default Auto goes here
  }
}

void Robot::TeleopInit() {}

void Robot::TeleopPeriodic() {}

void Robot::DisabledInit() {}

void Robot::DisabledPeriodic() {}

void Robot::UtilityInit() {}

void Robot::UtilityPeriodic() {}

void Robot::SimulationInit() {}

void Robot::SimulationPeriodic() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
