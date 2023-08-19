package edu.wpi.first.wpilibj2.command;

import java.util.function.BooleanSupplier;

/**
 * A command that runs nothing. Has no end condition as-is; either subclass it or
 * use {@link Command#withTimeout(double)} or {@link Command#until(BooleanSupplier)} to give it one.
 * Used mainly for making a {@link Subsystem#setDefaultCommand(Command)} only run once
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class IdleCommand extends RunCommand {
    /**
     * Creates a new IdleCommand. The IdleCommand will run until interrupted without doing any action.
     * Useful for making a continuously scheduled command only run once.
     *
     * @param requirements the subsystems to require
     */
    public IdleCommand(Subsystem... requirements){
        super(()-> {}, requirements);
    }
}
