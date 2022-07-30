// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/XboxController.h>
#include <frc/command/Command.h>
#include <frc/command/ConditionalCommand.h>
#include <frc/command/InstantCommand.h>
#include <frc/command/ParallelRaceGroup.h>
#include <frc/command/RunCommand.h>
#include <frc/command/SequentialCommandGroup.h>
#include <frc/command/WaitCommand.h>
#include <frc/command/WaitUntilCommand.h>
#include <frc/smartdashboard/SendableChooser.h>

#include "Constants.h"
#include "subsystems/DriveSubsystem.h"
#include "subsystems/ShooterSubsystem.h"

namespace ac = AutoConstants;

/**
 * This class is where the bulk of the robot should be declared.  Since
 * Command-based is a "declarative" paradigm, very little robot logic should
 * actually be handled in the {@link Robot} periodic methods (other than the
 * scheduler calls).  Instead, the structure of the robot (including subsystems,
 * commands, and button mappings) should be declared here.
 */
class RobotContainer {
 public:
  RobotContainer();

  frc::Command* GetAutonomousCommand();

 private:
  // The driver's controller
  frc::XboxController m_driverController{OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  ShooterSubsystem m_shooter;

  // A simple autonomous routine that shoots the loaded frisbees
  frc::SequentialCommandGroup m_autonomousCommand =
      frc::SequentialCommandGroup{
          // Start the command by spinning up the shooter...
          frc::InstantCommand([this] { m_shooter.Enable(); }, {&m_shooter}),
          // Wait until the shooter is at speed before feeding the frisbees
          frc::WaitUntilCommand([this] { return m_shooter.AtSetpoint(); }),
          // Start running the feeder
          frc::InstantCommand([this] { m_shooter.RunFeeder(); }, {&m_shooter}),
          // Shoot for the specified time
          frc::WaitCommand(ac::kAutoShootTimeSeconds)}
          // Add a timeout (will end the command if, for instance, the shooter
          // never gets up to speed)
          .WithTimeout(ac::kAutoTimeoutSeconds)
          // When the command ends, turn off the shooter and the feeder
          .AndThen([this] {
            m_shooter.Disable();
            m_shooter.StopFeeder();
          });

  // Assorted commands to be bound to buttons

  frc::InstantCommand m_spinUpShooter{[this] { m_shooter.Enable(); },
                                      {&m_shooter}};

  frc::InstantCommand m_stopShooter{[this] { m_shooter.Disable(); },
                                    {&m_shooter}};

  // Shoots if the shooter wheel has reached the target speed
  frc::ConditionalCommand m_shoot{
      // Run the feeder
      frc::InstantCommand{[this] { m_shooter.RunFeeder(); }, {&m_shooter}},
      // Do nothing
      frc::InstantCommand(),
      // Determine which of the above to do based on whether the shooter has
      // reached the desired speed
      [this] { return m_shooter.AtSetpoint(); }};

  frc::InstantCommand m_stopFeeder{[this] { m_shooter.StopFeeder(); },
                                   {&m_shooter}};

  frc::InstantCommand m_driveHalfSpeed{[this] { m_drive.SetMaxOutput(0.5); },
                                       {}};
  frc::InstantCommand m_driveFullSpeed{[this] { m_drive.SetMaxOutput(1); }, {}};

  // The chooser for the autonomous routines

  void ConfigureButtonBindings();
};
