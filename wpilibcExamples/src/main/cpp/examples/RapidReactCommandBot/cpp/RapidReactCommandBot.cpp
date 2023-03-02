// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RapidReactCommandBot.h"

#include <hal/simulation/MockHooks.h>
#include <frc/DriverStation.h>
#include <frc2/command/Command.h>
#include <frc2/command/Commands.h>
#include <frc2/command/ProxyCommand.h>
#include <frc2/command/button/Trigger.h>

#include "Constants.h"

void RapidReactCommandBot::RobotInit() {
  // Add the Scheduler as a periodic task. This is responsible for polling buttons, adding
  // newly-scheduled commands, running already-scheduled commands, removing finished or
  // interrupted commands, and running subsystem periodic() methods. This is required for anything
  // in the Command-based framework to work.
  AddPeriodic([] { frc2::CommandScheduler::GetInstance().Run(); }, GetPeriod());

  // Configure default commands and condition bindings on robot startup
  ConfigureBindings();
}

void RapidReactCommandBot::SimulationInit() {
  // Register the sim controllers to be run periodically in sim.
  HALSIM_RegisterSimPeriodicAfterCallback(ShooterSim::CallSimulationPeriodic, &m_shooterSim);
  HALSIM_RegisterSimPeriodicAfterCallback(DriveSim::CallSimulationPeriodic, &m_driveSim);
}

void RapidReactCommandBot::ConfigureBindings() {
  // Automatically run the storage motor whenever the ball storage is not full,
  // and turn it off whenever it fills. Uses subsystem-hosted trigger to
  // improve readability and make inter-subsystem communication easier.
  m_storage.HasCargo.WhileFalse(m_storage.RunCommand());

  // Automatically disable and retract the intake whenever the ball storage is
  // full.
  m_storage.HasCargo.OnTrue(m_intake.RetractCommand());

  // Control the drive with split-stick arcade controls
  m_drive.SetDefaultCommand(m_drive.ArcadeDriveCommand(
      [this] { return -m_driverController.GetLeftY(); },
      [this] { return -m_driverController.GetRightX(); }));

  // Deploy the intake with the X button
  m_driverController.X().OnTrue(m_intake.IntakeCommand());
  // Retract the intake with the Y button
  m_driverController.Y().OnTrue(m_intake.RetractCommand());

  // Fire the shooter with the A button
  m_driverController.A().OnTrue(
      frc2::cmd::Parallel(
          m_shooter.ShootCommand(ShooterConstants::kShooterTarget),
          m_storage.RunCommand())
          // Since we composed this inline we should give it a name
          .WithName("Shoot"));

  // Toggle compressor with the Start button
  m_driverController.Start().ToggleOnTrue(
      m_pneumatics.DisableCompressorCommand());
  frc2::Trigger([] { return frc::DriverStation::IsAutonomous();}).WhileTrue(frc2::cmd::Deferred([this] { return GetAutonomousCommand().AsProxy(); }));
}

frc2::CommandPtr RapidReactCommandBot::GetAutonomousCommand() {
  return m_drive
      .DriveDistanceCommand(AutoConstants::kDriveDistance,
                            AutoConstants::kDriveSpeed)
      .WithTimeout(AutoConstants::kTimeout);
}

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<RapidReactCommandBot>();
}
#endif
