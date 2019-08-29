/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Claw;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.DriveTrain;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Elevator;
import edu.wpi.first.wpilibj.examples.gearsbot.subsystems.Wrist;

/**
 * The main autonomous command to pickup and deliver the soda to the box.
 */
public class Autonomous extends SequentialCommandGroup {
  /**
   * Create a new autonomous command.
   */
  public Autonomous(DriveTrain drive, Claw claw, Wrist wrist, Elevator elevator) {
    addCommands(
        new PrepareToPickup(claw, wrist, elevator),
        new Pickup(claw, wrist, elevator),
        new SetDistanceToBox(0.10, drive),
        // new DriveStraight(4), // Use encoders if ultrasonic is broken
        new Place(claw, wrist, elevator),
        new SetDistanceToBox(0.60, drive),
        // new DriveStraight(-2), // Use Encoders if ultrasonic is broken
        parallel(
            new SetWristSetpoint(-45, wrist),
            new CloseClaw(claw)
        )
    );
  }
}
