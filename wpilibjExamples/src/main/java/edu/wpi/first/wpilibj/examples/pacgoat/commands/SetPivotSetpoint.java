/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.pacgoat.commands;

import edu.wpi.first.wpilibj.command.Command;

import edu.wpi.first.wpilibj.examples.pacgoat.Robot;

/**
 * Moves the pivot to a given angle. This command finishes when it is within the
 * tolerance, but leaves the PID loop running to maintain the position. Other
 * commands using the pivot should make sure they disable PID!
 */
public class SetPivotSetpoint extends Command {
	private double m_setpoint;

	public SetPivotSetpoint(double setpoint) {
		this.m_setpoint = setpoint;
		requires(Robot.pivot);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
		Robot.pivot.enable();
		Robot.pivot.setSetpoint(m_setpoint);
	}

	// Make this return true when this Command no longer needs to run execute()
	@Override
	protected boolean isFinished() {
		return Robot.pivot.onTarget();
	}
}
