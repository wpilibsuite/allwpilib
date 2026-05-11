// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "wpi/commands2/Commands.hpp"

RobotContainer::RobotContainer() {
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

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // While holding the bumper button, drive at half velocity
  driverController.RightBumper()
      .OnTrue(driveHalfVelocity.get())
      .OnFalse(driveFullVelocity.get());

  // Drive forward by 3 meters when the 'South Face' button is pressed, with a
  // timeout of 10 seconds
  driverController.SouthFace().OnTrue(
      drive.ProfiledDriveDistance(3_m).WithTimeout(10_s));

  // Do the same thing as above when the 'East Face' button is pressed, but
  // without resetting the encoders
  driverController.EastFace().OnTrue(
      drive.DynamicProfiledDriveDistance(3_m).WithTimeout(10_s));
}

wpi::cmd::CommandPtr RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return wpi::cmd::None();
}
