package $package.commands;

import $package.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 * Close the claw.
 * 
 * NOTE: It doesn't wait for the claw to close since there is no sensor to
 * detect that.
 */
public class CloseClaw extends Command {

    public CloseClaw() {
    	requires(Robot.collector);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
        Robot.collector.close();
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {
    }

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
