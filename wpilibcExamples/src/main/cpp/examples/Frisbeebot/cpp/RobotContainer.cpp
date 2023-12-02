// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

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

  // We can bind commands while keeping their ownership in RobotContainer

  // Spin up the shooter when the 'A' button is pressed
  m_driverController.A().OnTrue(m_spinUpShooter.get());

  // Turn off the shooter when the 'B' button is pressed
  m_driverController.B().OnTrue(m_stopShooter.get());

  // We can also *move* command ownership to the scheduler
  // Note that we won't be able to access these commands after moving them!

  // Shoots if the shooter wheel has reached the target speed
  frc2::CommandPtr shoot = frc2::cmd::Either(
      // Run the feeder
      frc2::cmd::RunOnce([this] { m_shooter.RunFeeder(); }, {&m_shooter}),
      // Do nothing
      frc2::cmd::None(),
      // Determine which of the above to do based on whether the shooter has
      // reached the desired speed
      [this] { return m_shooter.AtSetpoint(); });

  frc2::CommandPtr stopFeeder =
      frc2::cmd::RunOnce([this] { m_shooter.StopFeeder(); }, {&m_shooter});

  // Shoot when the 'X' button is pressed
  m_driverController.X()
      .OnTrue(std::move(shoot))
      .OnFalse(std::move(stopFeeder));

  // We can also define commands inline at the binding!
  // (ownership will be passed to the scheduler)

  // While holding the shoulder button, drive at half speed
  m_driverController.RightBumper()
      .OnTrue(frc2::cmd::RunOnce([this] { m_drive.SetMaxOutput(0.5); }, {}))
      .OnFalse(frc2::cmd::RunOnce([this] { m_drive.SetMaxOutput(1); }, {}));
}

frc2::CommandPtr RobotContainer::GetAutonomousCommand() {
  // Runs the chosen command in autonomous
  return frc2::cmd::Sequence(
          // Start the command by spinning up the shooter...
          frc2::cmd::RunOnce([this] { m_shooter.Enable(); }, {&m_shooter}),
          // Wait until the shooter is at speed before feeding the frisbees
          frc2::cmd::WaitUntil([this] { return m_shooter.AtSetpoint(); }),
          // Start running the feeder
          frc2::cmd::RunOnce([this] { m_shooter.RunFeeder(); }, {&m_shooter}),
          // Shoot for the specified time
          frc2::cmd::Wait(ac::kAutoShootTimeSeconds))
          // Add a timeout (will end the command if, for instance, the shooter
          // never gets up to speed)
          .WithTimeout(ac::kAutoTimeoutSeconds)
          // When the command ends, turn off the shooter and the feeder
          .AndThen(frc2::cmd::RunOnce([this] {
            m_shooter.Disable();
            m_shooter.StopFeeder();
          }));
}
