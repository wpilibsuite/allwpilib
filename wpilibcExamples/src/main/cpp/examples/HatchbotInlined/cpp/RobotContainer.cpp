// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "wpi/smartdashboard/SmartDashboard.hpp"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  // Note that we do *not* move ownership into the chooser
  chooser.SetDefaultOption("Simple Auto", simpleAuto.get());
  chooser.AddOption("Complex Auto", complexAuto.get());

  // Put the chooser on the dashboard
  wpi::SmartDashboard::PutData("Autonomous", &chooser);
  // Put subsystems to dashboard.
  wpi::SmartDashboard::PutData("Drivetrain", &drive);
  wpi::SmartDashboard::PutData("HatchSubsystem", &hatch);

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

  // Grab the hatch when the 'Face Right' button is pressed.
  driverController.FaceRight().OnTrue(hatch.GrabHatchCommand());
  // Release the hatch when the 'Face Left' button is pressed.
  driverController.FaceLeft().OnTrue(hatch.ReleaseHatchCommand());
  // While holding Right Bumper, drive at half velocity
  driverController.RightBumper()
      .OnTrue(wpi::cmd::RunOnce([this] { drive.SetMaxOutput(0.5); }, {}))
      .OnFalse(wpi::cmd::RunOnce([this] { drive.SetMaxOutput(1.0); }, {}));
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return chooser.GetSelected();
}
