// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbotcmdv3.commands;

import org.wpilib.command3.Command;
import org.wpilib.examples.hatchbotcmdv3.Constants.AutoConstants;
import org.wpilib.examples.hatchbotcmdv3.subsystems.DriveSubsystem;
import org.wpilib.examples.hatchbotcmdv3.subsystems.HatchSubsystem;

/** Container for auto command factories. */
public final class Autos {
  /** A simple auto routine that drives forward a specified distance, and then stops. */
  public static Command simpleAuto(DriveSubsystem drive) {
    return drive
        .run(
            coro -> {
              drive.resetEncoders();
              while (drive.getAverageEncoderDistance() >= AutoConstants.kAutoDriveDistanceInches) {
                drive.arcadeDrive(AutoConstants.kAutoDriveSpeed, 0);
                coro.yield();
              }
              drive.arcadeDrive(0, 0);
            })
        .whenCanceled(
            () -> {
              drive.arcadeDrive(0, 0);
            })
        .named("Simple Auto");
  }

  /** A complex auto routine that drives forward, drops a hatch, and then drives backward. */
  public static Command complexAuto(DriveSubsystem driveSubsystem, HatchSubsystem hatchSubsystem) {
    // NOTE: requirement behavior.
    // To require each mechanism for while it's active, replace `requiring` with `noRequirements`.
    return Command.requiring(driveSubsystem, hatchSubsystem)
        .executing(
            coro -> {
              // Drive forward up to the front of the cargo ship
              coro.await(
                  driveSubsystem
                      .run(
                          coro2 -> {
                            // Reset encoders on command start
                            driveSubsystem.resetEncoders();
                            // End the command when the robot's driven distance exceeds the desired
                            // value
                            while (driveSubsystem.getAverageEncoderDistance()
                                >= AutoConstants.kAutoDriveDistanceInches) {
                              // Drive forward while the command is executing
                              driveSubsystem.arcadeDrive(AutoConstants.kAutoDriveSpeed, 0);
                              coro2.yield();
                            }
                            // Stop driving at the end of the command
                            driveSubsystem.arcadeDrive(0, 0);
                          })
                      .whenCanceled(() -> driveSubsystem.arcadeDrive(0, 0))
                      .named("Part 1"));

              // Release the hatch
              coro.await(hatchSubsystem.releaseHatchCommand());

              // Drive backward the specified distance
              coro.await(
                  driveSubsystem
                      .run(
                          coro2 -> {
                            // Reset encoders on command start
                            driveSubsystem.resetEncoders();
                            // End the command when the robot's driven distance exceeds the desired
                            // value
                            while (driveSubsystem.getAverageEncoderDistance()
                                >= AutoConstants.kAutoDriveDistanceInches) {
                              // Drive backward while the command is executing
                              driveSubsystem.arcadeDrive(-AutoConstants.kAutoDriveSpeed, 0);
                              coro2.yield();
                            }
                            // Stop driving at the end of the command
                            driveSubsystem.arcadeDrive(0, 0);
                          })
                      .whenCanceled(() -> driveSubsystem.arcadeDrive(0, 0))
                      .named("Part 3"));
            })
        .named("Complex Auto");
  }

  private Autos() {
    throw new UnsupportedOperationException("This is a utility class!");
  }
}
