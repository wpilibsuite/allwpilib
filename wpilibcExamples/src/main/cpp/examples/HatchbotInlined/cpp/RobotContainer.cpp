// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.hpp"

#include <wpi/smartdashboard/SmartDashboard.hpp>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  // Note that we do *not* move ownership into the chooser
  m_chooser.SetDefaultOption("Simple Auto", m_simpleAuto.get());
  m_chooser.AddOption("Complex Auto", m_complexAuto.get());

  // Put the chooser on the dashboard
  wpi::SmartDashboard::PutData("Autonomous", &m_chooser);
  // Put subsystems to dashboard.
  wpi::SmartDashboard::PutData("Drivetrain", &m_drive);
  wpi::SmartDashboard::PutData("HatchSubsystem", &m_hatch);

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(wpi::cmd::cmd::Run(
      [this] {
        m_drive.ArcadeDrive(-m_driverController.GetLeftY(),
                            -m_driverController.GetRightX());
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // Grab the hatch when the 'Circle' button is pressed.
  m_driverController.Circle().OnTrue(m_hatch.GrabHatchCommand());
  // Release the hatch when the 'Square' button is pressed.
  m_driverController.Square().OnTrue(m_hatch.ReleaseHatchCommand());
  // While holding R1, drive at half speed
  m_driverController.R1()
      .OnTrue(wpi::cmd::cmd::RunOnce([this] { m_drive.SetMaxOutput(0.5); }, {}))
      .OnFalse(
          wpi::cmd::cmd::RunOnce([this] { m_drive.SetMaxOutput(1.0); }, {}));
}

wpi::cmd::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return m_chooser.GetSelected();
}
