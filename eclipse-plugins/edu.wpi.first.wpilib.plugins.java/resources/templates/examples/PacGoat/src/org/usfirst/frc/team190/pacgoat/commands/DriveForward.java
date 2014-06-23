package $package.commands;

import $package.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 * This command drives the robot over a given distance with simple proportional
 * control This command will drive a given distance limiting to a maximum speed.
 */
public class DriveForward extends Command {
	private double driveForwardSpeed;
	private double distance;
	private final double tolerance = .1; // TODO: Was 5
	private double error;
	private final double Kp = -1.0 / 5.0;

	public DriveForward() {
		this(10, 0.5);
	}

	public DriveForward(double dist) {
		this(dist, 0.5);
	}

	public DriveForward(double dist, double maxSpeed) {
		requires(Robot.drivetrain);
		distance = dist;
		driveForwardSpeed = maxSpeed;
	}

	protected void initialize() {
		Robot.drivetrain.getRightEncoder().reset();
		setTimeout(2);
	}

	protected void execute() {
		error = (distance - Robot.drivetrain.getRightEncoder().getDistance());
		if (driveForwardSpeed * Kp * error >= driveForwardSpeed) {
			Robot.drivetrain.tankDrive(driveForwardSpeed, driveForwardSpeed);
		} else {
			Robot.drivetrain.tankDrive(driveForwardSpeed * Kp * error,
					driveForwardSpeed * Kp * error);
		}
	}

	protected boolean isFinished() {
		return (Math.abs(error) <= tolerance) || isTimedOut();
	}

	protected void end() {
		Robot.drivetrain.stop();
	}

	protected void interrupted() {
		end();
	}
}
