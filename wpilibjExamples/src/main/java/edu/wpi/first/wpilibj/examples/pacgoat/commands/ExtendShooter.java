/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.TimedCommand;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Extend the shooter and then retract it after a second.
 */
public class ExtendShooter extends TimedCommand {
  public ExtendShooter() {
    super(1);
    requires(Robot.shooter);
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.shooter.extendBoth();
  }

  // Called once after isFinished returns true
  @Override
  protected void end() {
    Robot.shooter.retractBoth();
  }
}
