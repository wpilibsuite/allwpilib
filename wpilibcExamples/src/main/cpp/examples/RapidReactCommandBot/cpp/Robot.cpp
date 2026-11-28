// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Robot.hpp"

#include "Constants.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/button/Trigger.hpp"

Robot::Robot() {
  // Configure default commands and condition bindings on robot startup
  ConfigureBindings();
}

void Robot::ConfigureBindings() {
  // Automatically run the storage motor whenever the ball storage is not full,
  // and turn it off whenever it fills. Uses subsystem-hosted trigger to
  // improve readability and make inter-subsystem communication easier.
  storage.HasCargo.WhileFalse(storage.RunCommand());

  // Automatically disable and retract the intake whenever the ball storage is
  // full.
  storage.HasCargo.OnTrue(intake.RetractCommand());

  // Control the drive with split-stick arcade controls
  drive.SetDefaultCommand(drive.ArcadeDriveCommand(
      [this] { return -driverController.GetLeftY(); },
      [this] { return -driverController.GetRightX(); }));

  // Deploy the intake with the Face Left button
  driverController.FaceLeft().OnTrue(intake.IntakeCommand());
  // Retract the intake with the Face Up button
  driverController.FaceUp().OnTrue(intake.RetractCommand());

  // Fire the shooter with the Face Down button
  driverController.FaceDown().OnTrue(
      wpi::cmd::Parallel(shooter.ShootCommand(ShooterConstants::SHOOTER_TARGET),
                         storage.RunCommand())
          // Since we composed this inline we should give it a name
          .WithName("Shoot"));

  // Toggle compressor with the Start button
  driverController.Start().ToggleOnTrue(pneumatics.DisableCompressorCommand());
}

void Robot::RobotPeriodic() {
  // Runs the Scheduler.  This is responsible for polling buttons, adding
  // newly-scheduled commands, running already-scheduled commands, removing
  // finished or interrupted commands, and running subsystem Periodic() methods.
  // This must be called from the robot's periodic block in order for anything
  // in the Command-based framework to work.
  wpi::cmd::CommandScheduler::GetInstance().Run();
}

void Robot::DisabledStart() {}

void Robot::DisabledPeriodic() {}

void Robot::AutonomousStart() {
  // Drive forward for 2 meters at half velocity with a 3 second timeout
  autonomousCommand = drive
                          .DriveDistanceCommand(AutoConstants::DRIVE_DISTANCE,
                                                AutoConstants::DRIVE_VELOCITY)
                          .WithTimeout(AutoConstants::TIMEOUT);
  wpi::cmd::CommandScheduler::GetInstance().Schedule(autonomousCommand.value());
}

void Robot::AutonomousPeriodic() {}

void Robot::AutonomousEnd() {
  // This makes sure that the autonomous stops running when
  // autonomous mode ends. If you want the autonomous to
  // continue until interrupted by another command, remove
  // this line or comment it out.
  if (autonomousCommand) {
    autonomousCommand->Cancel();
  }
}

void Robot::TeleopStart() {}

void Robot::TeleopPeriodic() {}

void Robot::UtilityStart() {
  // Cancels all running commands at the start of utility mode.
  wpi::cmd::CommandScheduler::GetInstance().CancelAll();
}

void Robot::UtilityPeriodic() {}

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif
