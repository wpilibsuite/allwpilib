/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Drive over the line and then shoot the ball. If the hot goal is not detected,
 * it will wait briefly.
 */
public class DriveAndShootAutonomous extends CommandGroup {
  /**
   * Create a new drive and shoot autonomous.
   */
  public DriveAndShootAutonomous() {
    addSequential(new CloseClaw());
    addSequential(new WaitForPressure(), 2);
    if (Robot.isReal()) {
      // NOTE: Simulation doesn't currently have the concept of hot.
      addSequential(new CheckForHotGoal(2));
    }
    addSequential(new SetPivotSetpoint(45));
    addSequential(new DriveForward(8, 0.3));
    addSequential(new Shoot());
  }
}
