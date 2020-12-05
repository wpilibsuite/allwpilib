// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/XboxController.h>
#include <frc2/command/Command.h>
#include <frc2/command/FunctionalCommand.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/ParallelRaceGroup.h>
#include <frc2/command/RunCommand.h>
#include <frc2/command/SequentialCommandGroup.h>

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

  frc2::Command* GetAutonomousCommand();

 private:
  // The driver's controller
  frc::XboxController m_driverController{OIConstants::kDriverControllerPort};

  // The robot's subsystems and commands are defined here...

  // The robot's subsystems
  DriveSubsystem m_drive;
  HatchSubsystem m_hatch;

  // The autonomous routines
  frc2::FunctionalCommand m_simpleAuto = frc2::FunctionalCommand(
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

  frc2::InstantCommand m_grabHatch{[this] { m_hatch.GrabHatch(); }, {&m_hatch}};
  frc2::InstantCommand m_releaseHatch{[this] { m_hatch.ReleaseHatch(); },
                                      {&m_hatch}};
  frc2::InstantCommand m_driveHalfSpeed{[this] { m_drive.SetMaxOutput(0.5); },
                                        {}};
  frc2::InstantCommand m_driveFullSpeed{[this] { m_drive.SetMaxOutput(1); },
                                        {}};

  void ConfigureButtonBindings();
};
