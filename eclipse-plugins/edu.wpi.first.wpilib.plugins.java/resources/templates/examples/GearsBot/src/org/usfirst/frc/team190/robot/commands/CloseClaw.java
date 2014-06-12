/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package $package.commands;

import edu.wpi.first.wpilibj.command.Command;
import $package.Robot;

/**
 * Closes the claw for one second. Real robots should use sensors, stalling
 * motors is BAD!
 */
public class CloseClaw extends Command {
    
    public CloseClaw() {
        requires(Robot.claw);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
        Robot.claw.close();
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {
    }

    // Make this return true when this Command no longer needs to run execute()
    protected boolean isFinished() {
        return Robot.claw.isGrabbing();
    }

    // Called once after isFinished returns true
    protected void end() {
    	// NOTE: Doesn't stop in simulation due to lower friction causing the can to fall out
    	// + there is no need to worry about stalling the motor or crushing the can.
        if (!Robot.isSimulation()) 
        	Robot.claw.stop();
    }

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
    protected void interrupted() {
        end();
    }
}
