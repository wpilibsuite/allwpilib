package $package.commands;

import $package.subsystems.Collector;
import $package.subsystems.Pivot;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Get the robot set to collect balls.
 */
public class Collect extends CommandGroup {
    public Collect() {
    	addSequential(new SetCollectionSpeed(Collector.FORWARD));
    	addParallel(new CloseClaw());
    	addSequential(new SetPivotSetpoint(Pivot.COLLECT));
    	addSequential(new WaitForBall());
    }
}
