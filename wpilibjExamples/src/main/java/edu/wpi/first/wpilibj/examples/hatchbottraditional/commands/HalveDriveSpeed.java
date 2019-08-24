/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.DriveSubsystem;

public class HalveDriveSpeed extends CommandBase {
  private final DriveSubsystem m_drive;

  public HalveDriveSpeed(DriveSubsystem drive) {
    m_drive = drive;
  }

  @Override
  public void initialize() {
    m_drive.setMaxOutput(.5);
  }

  @Override
  public void end(boolean interrupted) {
    m_drive.setMaxOutput(1);
  }
}
