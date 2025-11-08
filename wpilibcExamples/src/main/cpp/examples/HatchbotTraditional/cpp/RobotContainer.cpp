// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include <wpi/commands2/button/JoystickButton.hpp>
#include <wpi/smartdashboard/SmartDashboard.hpp>

#include "commands/DefaultDrive.hpp"
#include "commands/GrabHatch.hpp"
#include "commands/HalveDriveSpeed.hpp"
#include "commands/ReleaseHatch.hpp"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  m_chooser.SetDefaultOption("Simple Auto", &m_simpleAuto);
  m_chooser.AddOption("Complex Auto", &m_complexAuto);

  // Put the chooser on the dashboard
  frc::SmartDashboard::PutData("Autonomous", &m_chooser);
  // Put subsystems to dashboard.
  frc::SmartDashboard::PutData("Drivetrain", &m_drive);
  frc::SmartDashboard::PutData("HatchSubsystem", &m_hatch);

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(DefaultDrive(
      &m_drive, [this] { return -m_driverController.GetLeftY(); },
      [this] { return -m_driverController.GetRightX(); }));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // NOTE: since we're binding a CommandPtr, command ownership here is moved to
  // the scheduler thus, no memory leaks!

  // Grab the hatch when the 'A' button is pressed.
  frc2::JoystickButton(&m_driverController, frc::XboxController::Button::kA)
      .OnTrue(GrabHatch(&m_hatch).ToPtr());
  // Release the hatch when the 'B' button is pressed.
  frc2::JoystickButton(&m_driverController, frc::XboxController::Button::kB)
      .OnTrue(ReleaseHatch(&m_hatch).ToPtr());
  // While holding the shoulder button, drive at half speed
  frc2::JoystickButton(&m_driverController,
                       frc::XboxController::Button::kRightBumper)
      .WhileTrue(HalveDriveSpeed(&m_drive).ToPtr());
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return m_chooser.GetSelected();
}
