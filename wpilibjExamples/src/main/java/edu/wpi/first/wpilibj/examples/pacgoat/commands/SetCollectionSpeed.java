/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.InstantCommand;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * This command sets the collector rollers spinning at the given speed. Since
 * there is no sensor for detecting speed, it finishes immediately. As a result,
 * the spinners may still be adjusting their speed.
 */
public class SetCollectionSpeed extends InstantCommand {
  private final double m_speed;

  public SetCollectionSpeed(double speed) {
    requires(Robot.collector);
    this.m_speed = speed;
  }

  // Called just before this Command runs the first time
  @Override
  protected void initialize() {
    Robot.collector.setSpeed(m_speed);
  }
}
