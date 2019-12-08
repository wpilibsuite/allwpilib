/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;
import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Pivot;

/**
 * Get the robot set to collect balls.
 */
public class Collect extends SequentialCommandGroup {
  /**
   * Create a new collect command.
   */
  public Collect() {
    addCommands(
        parallel(new SetCollectionSpeed(Collector.kForward),
        new CloseClaw()),
        new SetPivotSetpoint(Pivot.kCollect),
        new WaitForBall());
  }
}
