/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Place a held soda can onto the platform.
 */
public class Place extends CommandGroup {
  /**
   * Create a new place command.
   */
  public Place() {
    addSequential(new SetElevatorSetpoint(0.25));
    addSequential(new SetWristSetpoint(0));
    addSequential(new OpenClaw());
  }
}
