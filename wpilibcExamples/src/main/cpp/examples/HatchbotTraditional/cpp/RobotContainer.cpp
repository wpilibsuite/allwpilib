// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include "commands/DefaultDrive.hpp"
#include "commands/GrabHatch.hpp"
#include "commands/HalveDriveVelocity.hpp"
#include "commands/ReleaseHatch.hpp"
#include "wpi/commands2/button/GamepadButton.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  chooser.SetDefaultOption("Simple Auto", &simpleAuto);
  chooser.AddOption("Complex Auto", &complexAuto);

  // Put the chooser on the dashboard
  wpi::SmartDashboard::PutData("Autonomous", &chooser);
  // Put subsystems to dashboard.
  wpi::SmartDashboard::PutData("Drivetrain", &drive);
  wpi::SmartDashboard::PutData("HatchSubsystem", &hatch);

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  drive.SetDefaultCommand(DefaultDrive(
      &drive, [this] { return -driverController.GetLeftY(); },
      [this] { return -driverController.GetRightX(); }));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // NOTE: since we're binding a CommandPtr, command ownership here is moved to
  // the scheduler thus, no memory leaks!

  // Grab the hatch when the 'South Face' button is pressed.
  wpi::cmd::GamepadButton(&driverController, wpi::Gamepad::Button::SOUTH_FACE)
      .OnTrue(GrabHatch(&hatch).ToPtr());
  // Release the hatch when the 'East Face' button is pressed.
  wpi::cmd::GamepadButton(&driverController, wpi::Gamepad::Button::EAST_FACE)
      .OnTrue(ReleaseHatch(&hatch).ToPtr());
  // While holding the bumper button, drive at half velocity
  wpi::cmd::GamepadButton(&driverController, wpi::Gamepad::Button::RIGHT_BUMPER)
      .WhileTrue(HalveDriveVelocity(&drive).ToPtr());
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return chooser.GetSelected();
}
