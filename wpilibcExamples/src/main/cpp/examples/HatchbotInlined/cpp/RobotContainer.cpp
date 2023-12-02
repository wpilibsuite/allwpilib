// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/shuffleboard/Shuffleboard.h>

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  // Note that we do *not* move ownership into the chooser
  m_chooser.SetDefaultOption("Simple Auto", autos::AutoEnum::kSimpleAuto);
  m_chooser.AddOption("Complex Auto", autos::AutoEnum::kComplexAuto);

  // Put the chooser on the dashboard
  frc::Shuffleboard::GetTab("Autonomous").Add(m_chooser);
  // Put subsystems to dashboard.
  frc::Shuffleboard::GetTab("Drivetrain").Add(m_drive);
  frc::Shuffleboard::GetTab("HatchSubsystem").Add(m_hatch);

  // Log Shuffleboard events for command initialize, execute, finish, interrupt
  frc2::CommandScheduler::GetInstance().OnCommandInitialize(
      [](const frc2::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command initialized", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });
  frc2::CommandScheduler::GetInstance().OnCommandExecute(
      [](const frc2::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command executed", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });
  frc2::CommandScheduler::GetInstance().OnCommandFinish(
      [](const frc2::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command finished", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });
  frc2::CommandScheduler::GetInstance().OnCommandInterrupt(
      [](const frc2::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command interrupted", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc2::cmd::Run(
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
      .OnTrue(frc2::cmd::RunOnce([this] { m_drive.SetMaxOutput(0.5); }, {}))
      .OnFalse(frc2::cmd::RunOnce([this] { m_drive.SetMaxOutput(1.0); }, {}));
}

frc2::CommandPtr RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  switch (m_chooser.GetSelected()) {
    case autos::AutoEnum::kSimpleAuto:
      return autos::SimpleAuto(&m_drive);
      break;
    case autos::AutoEnum::kComplexAuto:
      return autos::ComplexAuto(&m_drive, &m_hatch);
      break;
    default:
      return frc2::CommandPtr(nullptr);
      break;
  }
}
