// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/PS4Controller.h>
#include <frc/command/Command.h>
#include <frc/command/FunctionalCommand.h>
#include <frc/command/InstantCommand.h>
#include <frc/command/ParallelRaceGroup.h>
#include <frc/command/RunCommand.h>
#include <frc/command/SequentialCommandGroup.h>
#include <frc/smartdashboard/SendableChooser.h>

#include "Constants.h"
#include "commands/ComplexAuto.h"
#include "subsystems/DriveSubsystem.h"
#include "subsystems/HatchSubsystem.h"

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
  frc::PS4Controller m_driverController{OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  HatchSubsystem m_hatch;

  // The autonomous routines
  frc::FunctionalCommand m_simpleAuto = frc::FunctionalCommand(
      // Reset encoders on command start
      [this] { m_drive.ResetEncoders(); },
      // Drive forward while the command is executing
      [this] { m_drive.ArcadeDrive(AutoConstants::kAutoDriveSpeed, 0); },
      // Stop driving at the end of the command
      [this](bool interrupted) { m_drive.ArcadeDrive(0, 0); },
      // End the command when the robot's driven distance exceeds the desired
      // value
      [this] {
        return m_drive.GetAverageEncoderDistance() >=
               AutoConstants::kAutoDriveDistanceInches;
      },
      // Requires the drive subsystem
      {&m_drive});
  ComplexAuto m_complexAuto{&m_drive, &m_hatch};

  // Assorted commands to be bound to buttons

  frc::InstantCommand m_grabHatch{[this] { m_hatch.GrabHatch(); }, {&m_hatch}};
  frc::InstantCommand m_releaseHatch{[this] { m_hatch.ReleaseHatch(); },
                                     {&m_hatch}};
  frc::InstantCommand m_driveHalfSpeed{[this] { m_drive.SetMaxOutput(0.5); },
                                       {}};
  frc::InstantCommand m_driveFullSpeed{[this] { m_drive.SetMaxOutput(1); }, {}};

  // The chooser for the autonomous routines
  frc::SendableChooser<frc::Command*> m_chooser;

  void ConfigureButtonBindings();
};
