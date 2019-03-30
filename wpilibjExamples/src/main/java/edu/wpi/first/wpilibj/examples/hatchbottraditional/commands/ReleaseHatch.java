package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj.experimental.command.InstantCommand;

/**
 * A command that releases the hatch.
 */
public class ReleaseHatch extends InstantCommand {
  public ReleaseHatch(HatchSubsystem subsystem) {
    super(subsystem::releaseHatch, subsystem);
  }
}
