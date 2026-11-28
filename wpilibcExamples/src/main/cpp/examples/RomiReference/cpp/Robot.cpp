// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "commands/TeleopArcadeDrive.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/tunables/Tunables.hpp"

Robot::Robot() {
  // Configure the button bindings
  ConfigureButtonBindings();
}

void Robot::ConfigureButtonBindings() {
  // Also set default commands here
  drive.SetDefaultCommand(TeleopArcadeDrive(
      &drive, [this] { return -controller.GetRawAxis(1); },
      [this] { return -controller.GetRawAxis(2); }));

  // Example of how to use the onboard IO
  onboardButtonA.OnTrue(wpi::cmd::Print("Button A Pressed"))
      .OnFalse(wpi::cmd::Print("Button A Released"));

  // Setup chooser options.
  chooser.AddDefault("Auto Routine Distance", &autoDistance);
  chooser.Add("Auto Routine Time", &autoTime);
  wpi::tunables::Publish("Auto Selector", chooser);
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
 * This autonomous runs the autonomous command selected by the chooser.
 */
void Robot::AutonomousStart() {
  autonomousCommand = chooser.GetSelected();
  wpi::cmd::CommandScheduler::GetInstance().Schedule(autonomousCommand);
}

void Robot::AutonomousPeriodic() {}

void Robot::AutonomousEnd() {
  // This makes sure that the autonomous stops running when
  // autonomous mode ends. If you want the autonomous to
  // continue until interrupted by another command, remove
  // this line or comment it out.
  if (autonomousCommand != nullptr) {
    autonomousCommand->Cancel();
  }
}

void Robot::TeleopStart() {}

/**
 * This function is called periodically during teleoperated mode.
 */
void Robot::TeleopPeriodic() {}

/**
 * This function is called periodically during utility mode.
 */
void Robot::UtilityPeriodic() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
