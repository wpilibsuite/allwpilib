/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;

/**
 * Shoot the ball at the current angle.
 */
public class Shoot extends CommandGroup {
  /**
   * Create a new shoot command.
   */
  public Shoot() {
    addSequential(new WaitForPressure());
    addSequential(new SetCollectionSpeed(Collector.kStop));
    addSequential(new OpenClaw());
    addSequential(new ExtendShooter());
  }
}
