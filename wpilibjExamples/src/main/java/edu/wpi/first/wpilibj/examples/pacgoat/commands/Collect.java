// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pivot;

/** Get the robot set to collect balls. */
public class Collect extends CommandGroup {
  /** Create a new collect command. */
  public Collect() {
    addSequential(new SetCollectionSpeed(Collector.kForward));
    addParallel(new CloseClaw());
    addSequential(new SetPivotSetpoint(Pivot.kCollect));
    addSequential(new WaitForBall());
  }
}
