package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj.experimental.command.SequentialCommandGroup;

import static edu.wpi.first.wpilibj.examples.hatchbottraditional.Constants.AutoConstants.*;

/**
 * A complex auto command that drives forward, releases a hatch, and then drives backward.
 */
public class ComplexAuto extends SequentialCommandGroup {

  public ComplexAuto(DriveSubsystem drive, HatchSubsystem hatch) {
    addCommands(
        // Drive forward the correct distance
        new DriveDistance(kAutoDriveDistanceInches, kAutoDriveSpeed, drive),

        // Release the hatch
        new ReleaseHatch(hatch),

        // Drive backward the specified distance
        new DriveDistance(kAutoBackupDistanceInches, -kAutoDriveSpeed, drive)
    );
  }

}
