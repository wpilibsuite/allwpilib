// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command2/CommandPtr.h>
#include <frc/command2/button/CommandXboxController.h>

#include "Constants.h"
#include "subsystems/Drive.h"
#include "subsystems/Intake.h"
#include "subsystems/Pneumatics.h"
#include "subsystems/Shooter.h"
#include "subsystems/Storage.h"

/**
 * This class is where the bulk of the robot should be declared. Since
 * Command-based is a "declarative" paradigm, very little robot logic should
 * actually be handled in the {@link Robot} periodic methods (other than the
 * scheduler calls). Instead, the structure of the robot (including subsystems,
 * commands, and button mappings) should be declared here.
 */
class RapidReactCommandBot {
 public:
  /**
   * Use this method to define bindings between conditions and commands. These
   * are useful for automating robot behaviors based on button and sensor input.
   *
   * <p>Should be called during Robot::RobotInit().
   *
   * <p>Event binding methods are available on the frc::Trigger class.
   */
  void ConfigureBindings();

  /**
   * Use this to define the command that runs during autonomous.
   *
   * <p>Scheduled during Robot::AutonomousInit().
   */
  frc::CommandPtr GetAutonomousCommand();

 private:
  // The robot's subsystems
  Drive m_drive;
  Intake m_intake;
  Shooter m_shooter;
  Storage m_storage;
  Pneumatics m_pneumatics;

  // The driver's controller
  frc::CommandXboxController m_driverController{
      OIConstants::kDriverControllerPort};
};
