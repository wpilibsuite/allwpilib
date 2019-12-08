/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.subsystems.Collector;

/**
 * Shoot the ball at the current angle.
 */
public class Shoot extends SequentialCommandGroup {
  /**
   * Create a new shoot command.
   */
  public Shoot() {
    addCommands(
        new WaitForPressure(),
        new SetCollectionSpeed(Collector.kStop),
        new OpenClaw(),
        new ExtendShooter()
    );
  }
}
