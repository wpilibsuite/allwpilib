package $package.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * Pickup a soda can (if one is between the open claws) and
 * get it in a safe state to drive around.
 */
public class Pickup extends CommandGroup {
    public  Pickup() {
    	addSequential(new CloseClaw());
    	addParallel(new SetWristSetpoint(-45));
    	addSequential(new SetElevatorSetpoint(0.25));
    }
}
