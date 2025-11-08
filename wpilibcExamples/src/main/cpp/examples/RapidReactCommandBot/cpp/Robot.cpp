// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

Robot::Robot() {
  // Configure default commands and condition bindings on robot startup
  m_robot.ConfigureBindings();
}

void Robot::RobotPeriodic() {
  // Runs the Scheduler.  This is responsible for polling buttons, adding
  // newly-scheduled commands, running already-scheduled commands, removing
  // finished or interrupted commands, and running subsystem Periodic() methods.
  // This must be called from the robot's periodic block in order for anything
  // in the Command-based framework to work.
  wpi::cmd::CommandScheduler::GetInstance().Run();
}

void Robot::DisabledInit() {}

void Robot::DisabledPeriodic() {}

void Robot::AutonomousInit() {
  m_autonomousCommand = m_robot.GetAutonomousCommand();

  if (m_autonomousCommand) {
    wpi::cmd::CommandScheduler::GetInstance().Schedule(m_autonomousCommand.value());
  }
}

void Robot::AutonomousPeriodic() {}

void Robot::TeleopInit() {
  // This makes sure that the autonomous stops running when
  // teleop starts running. If you want the autonomous to
  // continue until interrupted by another command, remove
  // this line or comment it out.
  if (m_autonomousCommand) {
    m_autonomousCommand->Cancel();
  }
}

void Robot::TeleopPeriodic() {}

void Robot::TestInit() {
  // Cancels all running commands at the start of test mode.
  wpi::cmd::CommandScheduler::GetInstance().CancelAll();
}

void Robot::TestPeriodic() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
