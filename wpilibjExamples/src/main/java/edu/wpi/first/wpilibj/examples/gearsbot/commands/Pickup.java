// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

/**
 * Pickup a soda can (if one is between the open claws) and get it in a safe state to drive around.
 */
public class Pickup extends SequentialCommandGroup {
  /**
   * Create a new pickup command.
   *
   * @param claw The claw subsystem to use
   * @param wrist The wrist subsystem to use
   * @param elevator The elevator subsystem to use
   */
  public Pickup(Claw claw, Wrist wrist, Elevator elevator) {
    addCommands(
        new CloseClaw(claw),
        parallel(new SetWristSetpoint(-45, wrist), new SetElevatorSetpoint(0.25, elevator)));
  }
}
