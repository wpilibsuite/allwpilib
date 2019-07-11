/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
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
