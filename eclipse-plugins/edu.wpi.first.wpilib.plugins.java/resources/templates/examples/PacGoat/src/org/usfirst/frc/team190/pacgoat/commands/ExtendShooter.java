package $package.commands;

import $package.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 * Extend the shooter and then retract it after a second.
 */
public class ExtendShooter extends Command {

    public ExtendShooter() {
    	requires(Robot.shooter);
    	setTimeout(1);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
    	Robot.shooter.extendBoth();
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {
    }

    // Make this return true when this Command no longer needs to run execute()
    protected boolean isFinished() {
        return isTimedOut();
    }

    // Called once after isFinished returns true
    protected void end() {
    	Robot.shooter.retractBoth();
    }

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
    protected void interrupted() {
    	end();
    }
}
