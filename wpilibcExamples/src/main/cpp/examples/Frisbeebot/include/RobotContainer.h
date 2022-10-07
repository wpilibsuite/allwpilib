// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/XboxController.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <frc2/command/Command.h>
#include <frc2/command/ConditionalCommand.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/ParallelRaceGroup.h>
#include <frc2/command/RunCommand.h>
#include <frc2/command/SequentialCommandGroup.h>
#include <frc2/command/WaitCommand.h>
#include <frc2/command/WaitUntilCommand.h>

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

  frc2::Command* GetAutonomousCommand();

 private:
  // The driver's controller
  frc::XboxController m_driverController{OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  ShooterSubsystem m_shooter;

  // A simple autonomous routine that shoots the loaded frisbees
  frc2::CommandPtr m_autonomousCommand =
      frc2::SequentialCommandGroup{
          // Start the command by spinning up the shooter...
          frc2::InstantCommand([this] { m_shooter.Enable(); }, {&m_shooter}),
          // Wait until the shooter is at speed before feeding the frisbees
          frc2::WaitUntilCommand([this] { return m_shooter.AtSetpoint(); }),
          // Start running the feeder
          frc2::InstantCommand([this] { m_shooter.RunFeeder(); }, {&m_shooter}),
          // Shoot for the specified time
          frc2::WaitCommand(ac::kAutoShootTimeSeconds)}
          // Add a timeout (will end the command if, for instance, the shooter
          // never gets up to speed)
          .WithTimeout(ac::kAutoTimeoutSeconds)
          // When the command ends, turn off the shooter and the feeder
          .AndThen([this] {
            m_shooter.Disable();
            m_shooter.StopFeeder();
          });

  // Assorted commands to be bound to buttons

  frc2::InstantCommand m_spinUpShooter{[this] { m_shooter.Enable(); },
                                       {&m_shooter}};

  frc2::InstantCommand m_stopShooter{[this] { m_shooter.Disable(); },
                                     {&m_shooter}};

  // Shoots if the shooter wheel has reached the target speed
  frc2::ConditionalCommand m_shoot{
      // Run the feeder
      frc2::InstantCommand{[this] { m_shooter.RunFeeder(); }, {&m_shooter}},
      // Do nothing
      frc2::InstantCommand(),
      // Determine which of the above to do based on whether the shooter has
      // reached the desired speed
      [this] { return m_shooter.AtSetpoint(); }};

  frc2::InstantCommand m_stopFeeder{[this] { m_shooter.StopFeeder(); },
                                    {&m_shooter}};

  frc2::InstantCommand m_driveHalfSpeed{[this] { m_drive.SetMaxOutput(0.5); },
                                        {}};
  frc2::InstantCommand m_driveFullSpeed{[this] { m_drive.SetMaxOutput(1); },
                                        {}};

  // The chooser for the autonomous routines

  void ConfigureButtonBindings();
};
