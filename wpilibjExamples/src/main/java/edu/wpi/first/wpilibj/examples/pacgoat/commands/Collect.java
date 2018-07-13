/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pivot;

/**
 * Get the robot set to collect balls.
 */
public class Collect extends CommandGroup {
  /**
   * Create a new collect command.
   */
  public Collect() {
    addSequential(new SetCollectionSpeed(Collector.kForward));
    addParallel(new CloseClaw());
    addSequential(new SetPivotSetpoint(Pivot.kCollect));
    addSequential(new WaitForBall());
  }
}
