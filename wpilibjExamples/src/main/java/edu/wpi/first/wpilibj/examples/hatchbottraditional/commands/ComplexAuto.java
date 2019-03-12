/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.HatchSubsystem;

import static edu.wpi.first.wpilibj.examples.hatchbottraditional.Constants.AutoConstants.kAutoBackupDistanceInches;
import static edu.wpi.first.wpilibj.examples.hatchbottraditional.Constants.AutoConstants.kAutoDriveDistanceInches;
import static edu.wpi.first.wpilibj.examples.hatchbottraditional.Constants.AutoConstants.kAutoDriveSpeed;

/**
 * A complex auto command that drives forward, releases a hatch, and then drives backward.
 */
public class ComplexAuto extends SequentialCommandGroup {
  /**
   * Creates a new ComplexAuto.
   *
   * @param drive The drive subsystem this command will run on
   * @param hatch The hatch subsystem this command will run on
   */
  public ComplexAuto(DriveSubsystem drive, HatchSubsystem hatch) {
    addCommands(
        // Drive forward the specified distance
        new DriveDistance(kAutoDriveDistanceInches, kAutoDriveSpeed, drive),

        // Release the hatch
        new ReleaseHatch(hatch),

        // Drive backward the specified distance
        new DriveDistance(kAutoBackupDistanceInches, -kAutoDriveSpeed, drive)
    );
  }

}
