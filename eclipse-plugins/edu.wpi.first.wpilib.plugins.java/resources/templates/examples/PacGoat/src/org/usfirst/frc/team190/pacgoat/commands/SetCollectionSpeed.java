package $package.commands;

import $package.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 * This command sets the collector rollers spinning at the given speed. Since
 * there is no sensor for detecting speed, it finishes immediately. As a result,
 * the spinners may still be adjusting their speed.
 */
public class SetCollectionSpeed extends Command {
	private double speed;

	public SetCollectionSpeed(double speed) {
		requires(Robot.collector);
		this.speed = speed;
	}

	// Called just before this Command runs the first time
	protected void initialize() {
		Robot.collector.setSpeed(speed);
	}

	// Called repeatedly when this Command is scheduled to run
	protected void execute() {}

    // Make this return true when this Command no longer needs to run execute()
	protected boolean isFinished() {
		return true;
	}

    // Called once after isFinished returns true
	protected void end() {}

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
	protected void interrupted() {}
}
