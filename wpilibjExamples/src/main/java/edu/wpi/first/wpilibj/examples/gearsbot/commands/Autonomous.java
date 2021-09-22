// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Drivetrain;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;
import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

/** The main autonomous command to pickup and deliver the soda to the box. */
public class Autonomous extends SequentialCommandGroup {
  /** Create a new autonomous command. */
  public Autonomous(Drivetrain drive, Claw claw, Wrist wrist, Elevator elevator) {
    addCommands(
        new PrepareToPickup(claw, wrist, elevator),
        new Pickup(claw, wrist, elevator),
        new SetDistanceToBox(0.10, drive),
        // new DriveStraight(4), // Use encoders if ultrasonic is broken
        new Place(claw, wrist, elevator),
        new SetDistanceToBox(0.60, drive),
        // new DriveStraight(-2), // Use Encoders if ultrasonic is broken
        parallel(new SetWristSetpoint(-45, wrist), new CloseClaw(claw)));
  }
}
