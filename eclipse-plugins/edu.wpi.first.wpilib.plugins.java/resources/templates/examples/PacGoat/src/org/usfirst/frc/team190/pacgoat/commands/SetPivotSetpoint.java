/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package $package.commands;

import $package.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 * Moves the  pivot to a given angle. This command finishes when it is within
 * the tolerance, but leaves the PID loop running to maintain the position.
 * Other commands using the pivot should make sure they disable PID!
 */
public class SetPivotSetpoint extends Command {
    private double setpoint;
    
    public SetPivotSetpoint(double setpoint) {
        this.setpoint = setpoint;
        requires(Robot.pivot);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
        Robot.pivot.enable();
        Robot.pivot.setSetpoint(setpoint);
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {}

    // Make this return true when this Command no longer needs to run execute()
    protected boolean isFinished() {
        return Robot.pivot.onTarget();
    }

    // Called once after isFinished returns true
    protected void end() {}

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
    protected void interrupted() {}
}
