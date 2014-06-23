package $package.commands;

import $package.subsystems.Collector;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Shoot the ball at the current angle.
 */
public class Shoot extends CommandGroup {
    public  Shoot() {
    	addSequential(new WaitForPressure());
    	addSequential(new SetCollectionSpeed(Collector.STOP));
    	addSequential(new OpenClaw());
    	addSequential(new ExtendShooter());
    }
}
