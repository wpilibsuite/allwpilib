/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pivot;

/**
 * Spit the ball out into the low goal assuming that the robot is in front of
 * it.
 */
public class LowGoal extends CommandGroup {
  /**
   * Create a new low goal command.
   */
  public LowGoal() {
    addSequential(new SetPivotSetpoint(Pivot.kLowGoal));
    addSequential(new SetCollectionSpeed(Collector.kReverse));
    addSequential(new ExtendShooter());
  }
}
