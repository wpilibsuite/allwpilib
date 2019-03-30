package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.HatchSubsystem;
import edu.wpi.first.wpilibj.experimental.command.InstantCommand;

/**
 * A command that grabs the hatch.
 */
public class GrabHatch extends InstantCommand {
  public GrabHatch(HatchSubsystem subsystem) {
    super(subsystem::grabHatch, subsystem);
  }
}
