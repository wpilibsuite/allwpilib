/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Pickup a soda can (if one is between the open claws) and get it in a safe
 * state to drive around.
 */
public class Pickup extends CommandGroup {
  /**
   * Create a new pickup command.
   */
  public Pickup() {
    addSequential(new CloseClaw());
    addParallel(new SetWristSetpoint(-45));
    addSequential(new SetElevatorSetpoint(0.25));
  }
}
