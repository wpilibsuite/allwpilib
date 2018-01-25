/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Drive over the line and then shoot the ball. If the hot goal is not detected,
 * it will wait briefly.
 */
public class DriveAndShootAutonomous extends CommandGroup {
	public DriveAndShootAutonomous() {
		addSequential(new CloseClaw());
		addSequential(new WaitForPressure(), 2);
		if (Robot.isReal()) {
			// NOTE: Simulation doesn't currently have the concept of hot.
			addSequential(new CheckForHotGoal(2));
		}
		addSequential(new SetPivotSetpoint(45));
		addSequential(new DriveForward(8, 0.3));
		addSequential(new Shoot());
	}
}
