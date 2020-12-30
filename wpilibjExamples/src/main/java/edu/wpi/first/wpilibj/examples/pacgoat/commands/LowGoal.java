// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pivot;

/** Spit the ball out into the low goal assuming that the robot is in front of it. */
public class LowGoal extends CommandGroup {
  /** Create a new low goal command. */
  public LowGoal() {
    addSequential(new SetPivotSetpoint(Pivot.kLowGoal));
    addSequential(new SetCollectionSpeed(Collector.kReverse));
    addSequential(new ExtendShooter());
  }
}
