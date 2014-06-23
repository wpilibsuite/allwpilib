package $package.commands;

import $package.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 * This command looks for the hot goal and waits until it's detected or timed
 * out. The timeout is because it's better to shoot and get some autonomous
 * points than get none. When called sequentially, this command will block until
 * the hot goal is detected or until it is timed out.
 */
public class CheckForHotGoal extends Command {
	public CheckForHotGoal(double time) {
		setTimeout(time);
	}

	// Called just before this Command runs the first time
	protected void initialize() {}

    // Called repeatedly when this Command is scheduled to run
	protected void execute() {}

    // Make this return true when this Command no longer needs to run execute()
	protected boolean isFinished() {
		return isTimedOut() || Robot.shooter.goalIsHot();
	}
	
	// Called once after isFinished returns true
	protected void end() {}

	// Called when another command which requires one or more of the same
	// subsystems is scheduled to run
	protected void interrupted() {
		end();
	}
}
