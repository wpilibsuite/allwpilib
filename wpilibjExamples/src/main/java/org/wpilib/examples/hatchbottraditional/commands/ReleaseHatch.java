// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.hatchbottraditional.commands;

import org.wpilib.command2.InstantCommand;
import org.wpilib.examples.hatchbottraditional.subsystems.HatchSubsystem;

/** A command that releases the hatch. */
public class ReleaseHatch extends InstantCommand {
  public ReleaseHatch(HatchSubsystem subsystem) {
    super(subsystem::releaseHatch, subsystem);
  }
}
