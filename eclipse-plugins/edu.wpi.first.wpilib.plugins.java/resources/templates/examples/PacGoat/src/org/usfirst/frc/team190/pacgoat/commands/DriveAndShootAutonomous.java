package $package.commands;

import $package.Robot;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Drive over the line and then shoot the ball. If the hot goal is not detected,
 * it will wait briefly.
 */
public class DriveAndShootAutonomous extends CommandGroup {
    public DriveAndShootAutonomous() {
        addSequential(new WaitForPressure(), 2);
        if (Robot.isReal()) {
        	// NOTE: Simulation doesn't currently have the concept of hot. 
        	addSequential(new CheckForHotGoal(2));
        }	
        addSequential(new CloseClaw());
        addSequential(new SetPivotSetpoint(45));
        addSequential(new DriveForward(8, 0.4));
        addSequential(new Shoot());
    }
}
