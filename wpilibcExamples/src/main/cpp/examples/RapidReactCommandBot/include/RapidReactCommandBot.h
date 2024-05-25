// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/TimedRobot.h>
#include <frc2/command/CommandPtr.h>
#include <frc2/command/button/CommandXboxController.h>

#include "Constants.h"
#include "sim/DriveSim.h"
#include "sim/ShooterSim.h"
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
class RapidReactCommandBot : public frc::TimedRobot {
 public:
  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code. In this case, we set up all bindings and periodic tasks.
   */
  void RobotInit() override;

  /** Setup simulation. */
  void SimulationInit() override;

  /**
   * Use this method to define bindings between conditions and commands. These
   * are useful for automating robot behaviors based on button and sensor input.
   *
   * <p>Should be called during RobotInit().
   *
   * <p>Event binding methods are available on the frc2::Trigger class.
   */
  void ConfigureBindings();

  /**
   * Use this to define the command that runs during autonomous.
   *
   * <p>Scheduled during AutonomousInit().
   */
  frc2::CommandPtr GetAutonomousCommand();

 private:
  // The robot's subsystems
  Drive m_drive;
  Intake m_intake;
  Shooter m_shooter;
  Storage m_storage;
  Pneumatics m_pneumatics;

  // We need these as fields here so they don't get destroyed.
//   private SimPeriodicAfterCallback m_shooterSimCallback;
//   private SimPeriodicAfterCallback m_driveSimCallback;

  // Simulation controllers
  DriveSim m_driveSim;
  ShooterSim m_shooterSim;


  // The driver's controller
  frc2::CommandXboxController m_driverController{
      OIConstants::kDriverControllerPort};
};
