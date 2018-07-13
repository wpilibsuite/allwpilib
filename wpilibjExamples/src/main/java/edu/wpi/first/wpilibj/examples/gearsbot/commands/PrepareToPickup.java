/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Make sure the robot is in a state to pickup soda cans.
 */
public class PrepareToPickup extends CommandGroup {
  /**
   * Create a new prepare to pickup command.
   */
  public PrepareToPickup() {
    addParallel(new OpenClaw());
    addParallel(new SetWristSetpoint(0));
    addSequential(new SetElevatorSetpoint(0));
  }
}
