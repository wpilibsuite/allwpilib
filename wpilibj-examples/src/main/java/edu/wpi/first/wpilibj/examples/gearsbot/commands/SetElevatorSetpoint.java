/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * Move the elevator to a given location. This command finishes when it is
 * within the tolerance, but leaves the PID loop running to maintain the
 * position. Other commands using the elevator should make sure they disable
 * PID!
 */
public class SetElevatorSetpoint extends Command {
	private double setpoint;

	public SetElevatorSetpoint(double setpoint) {
		this.setpoint = setpoint;
		requires(Robot.elevator);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
		Robot.elevator.enable();
		Robot.elevator.setSetpoint(setpoint);
	}

	// Make this return true when this Command no longer needs to run execute()
	@Override
	protected boolean isFinished() {
		return Robot.elevator.onTarget();
	}
}
