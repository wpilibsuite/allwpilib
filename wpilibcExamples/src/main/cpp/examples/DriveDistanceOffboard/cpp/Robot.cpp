// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/Commands.hpp"

Robot::Robot() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  drive.SetDefaultCommand(wpi::cmd::Run(
      [this] {
        drive.ArcadeDrive(-driverController.GetLeftY(),
                          -driverController.GetRightX());
      },
      {&drive}));
}

void Robot::ConfigureButtonBindings() {
  // Configure your button bindings here

  // While holding the bumper button, drive at half velocity
  driverController.RightBumper()
      .OnTrue(driveHalfVelocity.get())
      .OnFalse(driveFullVelocity.get());

  // Drive forward by 3 meters when the 'Face Down' button is pressed, with a
  // timeout of 10 seconds
  driverController.FaceDown().OnTrue(
      drive.ProfiledDriveDistance(3_m).WithTimeout(10_s));

  // Do the same thing as above when the 'Face Right' button is pressed, but
  // without resetting the encoders
  driverController.FaceRight().OnTrue(
      drive.DynamicProfiledDriveDistance(3_m).WithTimeout(10_s));
}

/**
 * This function is called every 20 ms, no matter the mode. Use
 * this for items like diagnostics that you want to run during disabled,
 * autonomous, teleoperated and utility.
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
  autonomousCommand = wpi::cmd::None();
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
    autonomousCommand.reset();
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
