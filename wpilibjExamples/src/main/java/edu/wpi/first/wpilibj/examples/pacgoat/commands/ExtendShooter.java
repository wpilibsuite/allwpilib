/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj2.command.CommandBase;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Extend the shooter and then retract it after a second.
 */
public class ExtendShooter extends CommandBase {
  public ExtendShooter() {
    addRequirements(Robot.shooter);
  }

  // Called just before this Command runs the first time
  @Override
  public void initialize() {
    Robot.shooter.extendBoth();
  }

  // Called once after isFinished returns true
  @Override
  public void end(boolean interrupted) {
    Robot.shooter.retractBoth();
  }
}
