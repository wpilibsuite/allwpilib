/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Make sure the robot is in a state to pickup soda cans.
 */
public class PrepareToPickup extends CommandGroup {
	public PrepareToPickup() {
		addParallel(new OpenClaw());
		addParallel(new SetWristSetpoint(0));
		addSequential(new SetElevatorSetpoint(0));
	}
}
