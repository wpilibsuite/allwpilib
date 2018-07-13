/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Pickup a soda can (if one is between the open claws) and get it in a safe
 * state to drive around.
 */
public class Pickup extends CommandGroup {
  /**
   * Create a new pickup command.
   */
  public Pickup() {
    addSequential(new CloseClaw());
    addParallel(new SetWristSetpoint(-45));
    addSequential(new SetElevatorSetpoint(0.25));
  }
}
