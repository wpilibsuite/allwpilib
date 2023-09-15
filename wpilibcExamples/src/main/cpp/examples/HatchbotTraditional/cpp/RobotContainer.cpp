// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <frc/command2/button/JoystickButton.h>
#include <frc/shuffleboard/Shuffleboard.h>

#include "commands/DefaultDrive.h"
#include "commands/GrabHatch.h"
#include "commands/HalveDriveSpeed.h"
#include "commands/ReleaseHatch.h"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Add commands to the autonomous command chooser
  m_chooser.SetDefaultOption("Simple Auto", &m_simpleAuto);
  m_chooser.AddOption("Complex Auto", &m_complexAuto);

  // Put the chooser on the dashboard
  frc::Shuffleboard::GetTab("Autonomous").Add(m_chooser);
  // Put subsystems to dashboard.
  frc::Shuffleboard::GetTab("Drivetrain").Add(m_drive);
  frc::Shuffleboard::GetTab("HatchSubsystem").Add(m_hatch);

  // Log Shuffleboard events for command initialize, execute, finish, interrupt
  frc::CommandScheduler::GetInstance().OnCommandInitialize(
      [](const frc::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command initialized", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });
  frc::CommandScheduler::GetInstance().OnCommandExecute(
      [](const frc::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command executed", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });
  frc::CommandScheduler::GetInstance().OnCommandFinish(
      [](const frc::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command finished", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });
  frc::CommandScheduler::GetInstance().OnCommandInterrupt(
      [](const frc::Command& command) {
        frc::Shuffleboard::AddEventMarker(
            "Command interrupted", command.GetName(),
            frc::ShuffleboardEventImportance::kNormal);
      });

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
  frc::JoystickButton(&m_driverController, frc::XboxController::Button::kA)
      .OnTrue(GrabHatch(&m_hatch).ToPtr());
  // Release the hatch when the 'B' button is pressed.
  frc::JoystickButton(&m_driverController, frc::XboxController::Button::kB)
      .OnTrue(ReleaseHatch(&m_hatch).ToPtr());
  // While holding the shoulder button, drive at half speed
  frc::JoystickButton(&m_driverController,
                      frc::XboxController::Button::kRightBumper)
      .WhileTrue(HalveDriveSpeed(&m_drive).ToPtr());
}

frc::Command* RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return m_chooser.GetSelected();
}
