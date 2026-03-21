// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbottraditional.commands;

import org.wpilib.command2.Command;
import org.wpilib.examples.hatchbottraditional.subsystems.DriveSubsystem;

public class DriveDistance extends Command {
  private final DriveSubsystem drive;
  private final double distance;
  private final double velocity;

  /**
   * Creates a new DriveDistance.
   *
   * @param inches The number of inches the robot will drive
   * @param velocity The velocity at which the robot will drive
   * @param drive The drive subsystem on which this command will run
   */
  public DriveDistance(double inches, double velocity, DriveSubsystem drive) {
    distance = inches;
    this.velocity = velocity;
    this.drive = drive;
    addRequirements(drive);
  }

  @Override
  public void initialize() {
    drive.resetEncoders();
    drive.arcadeDrive(velocity, 0);
  }

  @Override
  public void execute() {
    drive.arcadeDrive(velocity, 0);
  }

  @Override
  public void end(boolean interrupted) {
    drive.arcadeDrive(0, 0);
  }

  @Override
  public boolean isFinished() {
    return Math.abs(drive.getAverageEncoderDistance()) >= distance;
  }
}
