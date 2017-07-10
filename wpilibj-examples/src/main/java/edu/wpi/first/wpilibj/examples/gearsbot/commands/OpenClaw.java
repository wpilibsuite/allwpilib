/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.wpi.first.wpilibj.examples.gearsbot.commands;

import edu.wpi.first.wpilibj.command.TimedCommand;
import edu.wpi.first.wpilibj.examples.gearsbot.Robot;

/**
 * Opens the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
public class OpenClaw extends TimedCommand {
	public OpenClaw() {
		super(1);
		requires(Robot.claw);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
		Robot.claw.open();
	}

	// Called once after isFinished returns true
	@Override
	protected void end() {
		Robot.claw.stop();
	}
}
