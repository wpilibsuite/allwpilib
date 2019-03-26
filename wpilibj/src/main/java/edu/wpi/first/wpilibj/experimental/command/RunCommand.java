package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.BooleanSupplier;

/**
 * A command that runs a Runnable continuously.  Has no end condition as-is;
 * either subclass it or use {@link Command#withTimeout(double)} or
 * {@link Command#interruptOn(BooleanSupplier)} to give it one.  If you only wish
 * to execute a Runnable once, use {@link InstantCommand}.
 */
public class RunCommand extends SendableCommandBase {

  protected Runnable m_toRun;

  /**
   * Creates a new RunCommand.  The Runnable will be run continuously until the command
   * ends.
   *
   * @param toRun        the Runnable to run
   * @param requirements the subsystems to require
   */
  public RunCommand(Runnable toRun, Subsystem... requirements) {
    m_toRun = toRun;
    addRequirements(requirements);
  }

  @Override
  public void execute() {
    m_toRun.run();
  }
}
