// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.examples.gearsbot.Constants;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj2.command.ParallelCommandGroup;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

/** Make sure the robot is in a state to pickup soda cans. */
public class PrepareToPickup extends SequentialCommandGroup {
  /**
   * Create a new prepare to pickup command.
   *
   * @param claw The claw subsystem to use
   * @param wrist The wrist subsystem to use
   * @param elevator The elevator subsystem to use
   */
  public PrepareToPickup(Claw claw, Wrist wrist, Elevator elevator) {
    addCommands(
      new OpenClaw(claw),
      new ParallelCommandGroup(
        new SetWristSetpoint(Constants.Positions.PrepareToPickup.wristSetpoint, wrist),
        new SetElevatorSetpoint(Constants.Positions.PrepareToPickup.elevatorSetpoint, elevator)
        )
      );
  }
}
