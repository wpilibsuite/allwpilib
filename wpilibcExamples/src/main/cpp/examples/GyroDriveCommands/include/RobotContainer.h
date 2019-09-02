/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/XboxController.h>
#include <frc/controller/PIDController.h>
#include <frc/smartdashboard/SendableChooser.h>
#include <frc2/command/Command.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/PIDCommand.h>
#include <frc2/command/ParallelRaceGroup.h>
#include <frc2/command/RunCommand.h>

#include "Constants.h"
#include "commands/TurnToAngle.h"
#include "subsystems/DriveSubsystem.h"

namespace ac = AutoConstants;
namespace dc = DriveConstants;

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

  // Assorted commands to be bound to buttons

  // Turn to 90 degrees, with a 5 second timeout
  frc2::ParallelRaceGroup m_turnTo90 =
      TurnToAngle{90, &m_drive}.WithTimeout(5_s);

  // Stabilize the robot while driving
  frc2::PIDCommand m_stabilizeDriving{
      frc2::PIDController{dc::kStabilizationP, dc::kStabilizationI,
                          dc::kStabilizationD},
      // Close the loop on the turn rate
      [this] { return m_drive.GetTurnRate(); },
      // Setpoint is 0
      0,
      // Pipe the output to the turning controls
      [this](double output) {
        m_drive.ArcadeDrive(
            m_driverController.GetY(frc::GenericHID::JoystickHand::kLeftHand),
            output);
      },
      // Require the robot drive
      {&m_drive}};

  frc2::InstantCommand m_driveHalfSpeed{[this] { m_drive.SetMaxOutput(.5); },
                                        {}};
  frc2::InstantCommand m_driveFullSpeed{[this] { m_drive.SetMaxOutput(1); },
                                        {}};

  // The chooser for the autonomous routines
  frc::SendableChooser<frc2::Command*> m_chooser;

  void ConfigureButtonBindings();
};
