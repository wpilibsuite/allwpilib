// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotcmdv3.commands;

import org.wpilib.command3.Command;
import org.wpilib.examples.hatchbotcmdv3.Constants.AutoConstants;
import org.wpilib.examples.hatchbotcmdv3.mechanisms.DriveMechanism;
import org.wpilib.examples.hatchbotcmdv3.mechanisms.HatchMechanism;

/** Container for auto command factories. */
public final class Autos {
  /** A simple auto routine that drives forward a specified distance and then stops. */
  public static Command simpleAuto(DriveMechanism drive) {
    return drive.driveDistance(
        AutoConstants.kAutoDriveDistanceInches, AutoConstants.kAutoDriveSpeed);
  }

  /** A complex auto routine that drives forward, drops a hatch, and then drives backward. */
  public static Command complexAuto(DriveMechanism driveMechanism, HatchMechanism hatchMechanism) {
    // NOTE: requirement behavior.
    // To require each mechanism for while it's active, replace `requiring` with `noRequirements`.
    return Command.requiring(driveMechanism, hatchMechanism)
        .executing(
            coro -> {
              // Drive forward up to the front of the cargo ship
              coro.await(
                  driveMechanism.driveDistance(
                      AutoConstants.kAutoDriveDistanceInches, AutoConstants.kAutoDriveSpeed));

              // Release the hatch
              coro.await(hatchMechanism.releaseHatchCommand());

              // Drive backward the specified distance
              coro.await(
                  driveMechanism.driveDistance(
                      AutoConstants.kAutoBackupDistanceInches, AutoConstants.kAutoDriveSpeed));
            })
        .named("Complex Auto");
  }

  private Autos() {
    throw new UnsupportedOperationException("This is a utility class!");
  }
}
