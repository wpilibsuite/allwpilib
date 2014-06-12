/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package $package.commands;

import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.PIDSource;
import edu.wpi.first.wpilibj.command.Command;
import $package.Robot;

/**
 * Drive until the robot is the given distance away from the box. Uses a local
 * PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
public class SetDistanceToBox extends Command {
	private PIDController pid;

	public SetDistanceToBox(double distance) {
		requires(Robot.drivetrain);
		pid = new PIDController(-2, 0, 0, new PIDSource() {
			public double pidGet() {
				return Robot.drivetrain.getDistanceToObstacle();
			}
		}, new PIDOutput() {
			public void pidWrite(double d) {
				Robot.drivetrain.drive(d, d);
			}
		});
		pid.setAbsoluteTolerance(0.01);
		pid.setSetpoint(distance);
	}

	// Called just before this Command runs the first time
	protected void initialize() {
		// Get everything in a safe starting state.
		Robot.drivetrain.reset();
		pid.reset();
		pid.enable();
	}

	// Called repeatedly when this Command is scheduled to run
	protected void execute() {
	}

	// Make this return true when this Command no longer needs to run execute()
	protected boolean isFinished() {
		return pid.onTarget();
	}

	// Called once after isFinished returns true
	protected void end() {
		// Stop PID and the wheels
		pid.disable();
		Robot.drivetrain.drive(0, 0);
	}

	// Called when another command which requires one or more of the same
	// subsystems is scheduled to run
	protected void interrupted() {
		end();
	}
}
