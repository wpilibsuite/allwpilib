// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbottraditional.commands;

import java.util.function.DoubleSupplier;
import org.wpilib.command2.Command;
import org.wpilib.examples.hatchbottraditional.subsystems.DriveSubsystem;

/**
 * A command to drive the robot with joystick input (passed in as {@link DoubleSupplier}s). Written
 * explicitly for pedagogical purposes - actual code should inline a command this simple with {@link
 * org.wpilib.command2.RunCommand}.
 */
public class DefaultDrive extends Command {
  private final DriveSubsystem drive;
  private final DoubleSupplier forward;
  private final DoubleSupplier rotation;

  /**
   * Creates a new DefaultDrive.
   *
   * @param subsystem The drive subsystem this command wil run on.
   * @param forward The control input for driving forwards/backwards
   * @param rotation The control input for turning
   */
  public DefaultDrive(DriveSubsystem subsystem, DoubleSupplier forward, DoubleSupplier rotation) {
    drive = subsystem;
    this.forward = forward;
    this.rotation = rotation;
    addRequirements(drive);
  }

  @Override
  public void execute() {
    drive.arcadeDrive(forward.getAsDouble(), rotation.getAsDouble());
  }
}
