// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "commands/Autos.hpp"
#include "commands/ExampleCommand.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/button/Trigger.hpp"

Robot::Robot() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureBindings();
}

void Robot::ConfigureBindings() {
  // Configure your trigger bindings here

  // Schedule `ExampleCommand` when `exampleCondition` changes to `true`
  wpi::cmd::Trigger([this] {
    return subsystem.ExampleCondition();
  }).OnTrue(ExampleCommand(&subsystem).ToPtr());

  // Schedule `ExampleMethodCommand` when the Gamepad's Face Right button is
  // pressed, cancelling on release.
  driverController.FaceRight().WhileTrue(subsystem.ExampleMethodCommand());
}

/**
 * This function is called every 20 ms, no matter the mode. Use
 * this for items like diagnostics that you want to run during disabled,
 * autonomous, teleoperated and utility.
 *
 * <p> This runs after the mode specific periodic functions, but before
 * LiveWindow and SmartDashboard integrated updating.
 */
void Robot::RobotPeriodic() {
  wpi::cmd::CommandScheduler::GetInstance().Run();
}

/**
 * This function is called each time Disabled mode starts. You
 * can use it to reset any subsystem information you want to clear when the
 * robot is disabled.
 */
void Robot::DisabledStart() {}

void Robot::DisabledPeriodic() {}

/**
 * This autonomous runs your autonomous command.
 */
void Robot::AutonomousStart() {
  // An example command will be run in autonomous
  autonomousCommand = autos::ExampleAuto(&subsystem);
  wpi::cmd::CommandScheduler::GetInstance().Schedule(autonomousCommand.value());
}

void Robot::AutonomousPeriodic() {}

void Robot::AutonomousEnd() {
  // This makes sure that the autonomous stops running when
  // autonomous mode ends. If you want the autonomous to
  // continue until interrupted by another command, remove
  // this line or comment it out.
  if (autonomousCommand) {
    autonomousCommand->Cancel();
  }
}

/**
 * This function is called each time teleoperated mode starts.
 */
void Robot::TeleopStart() {}

/**
 * This function is called periodically during teleoperated mode.
 */
void Robot::TeleopPeriodic() {}

/**
 * This function is called periodically during utility mode.
 */
void Robot::UtilityPeriodic() {}

/**
 * This function is called once when the robot is first started up.
 */
void Robot::SimulationInit() {}

/**
 * This function is called periodically whilst in simulation.
 */
void Robot::SimulationPeriodic() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
