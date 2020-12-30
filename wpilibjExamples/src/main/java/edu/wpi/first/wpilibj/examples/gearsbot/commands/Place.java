// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

/** Place a held soda can onto the platform. */
public class Place extends SequentialCommandGroup {
  /**
   * Create a new place command.
   *
   * @param claw The claw subsystem to use
   * @param wrist The wrist subsystem to use
   * @param elevator The elevator subsystem to use
   */
  public Place(Claw claw, Wrist wrist, Elevator elevator) {
    addCommands(
        new SetElevatorSetpoint(0.25, elevator),
        new SetWristSetpoint(0, wrist),
        new OpenClaw(claw));
  }
}
