// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
