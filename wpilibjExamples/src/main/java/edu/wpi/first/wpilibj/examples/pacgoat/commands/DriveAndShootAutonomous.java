/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj2.command.SequentialCommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Drive over the line and then shoot the ball. If the hot goal is not detected,
 * it will wait briefly.
 */
public class DriveAndShootAutonomous extends SequentialCommandGroup {
  /**
   * Create a new drive and shoot autonomous.
   */
  public DriveAndShootAutonomous() {
    if (Robot.isReal()) {
      addCommands(
          new CloseClaw(),
          new WaitForPressure().withTimeout(2),
          // NOTE: Simulation doesn't currently have the concept of hot.
          new CheckForHotGoal(2),
          new SetPivotSetpoint(45),
          new DriveForward(8, 0.3),
          new Shoot()
      );
    } else {
      addCommands(
          new CloseClaw(),
          new WaitForPressure().withTimeout(2),
          new SetPivotSetpoint(45),
          new DriveForward(8, 0.3),
          new Shoot()
      );
    }
  }
}
