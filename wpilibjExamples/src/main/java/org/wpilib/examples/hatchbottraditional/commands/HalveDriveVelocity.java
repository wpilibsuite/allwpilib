// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbottraditional.commands;

import org.wpilib.command2.Command;
import org.wpilib.examples.hatchbottraditional.subsystems.DriveSubsystem;

public class HalveDriveVelocity extends Command {
  private final DriveSubsystem m_drive;

  public HalveDriveVelocity(DriveSubsystem drive) {
    m_drive = drive;
  }

  @Override
  public void initialize() {
    m_drive.setMaxOutput(0.5);
  }

  @Override
  public void end(boolean interrupted) {
    m_drive.setMaxOutput(1);
  }
}
