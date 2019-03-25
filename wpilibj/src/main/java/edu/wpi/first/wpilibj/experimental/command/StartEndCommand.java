package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.BooleanSupplier;

/**
 * A command that runs a given runnable when it is initalized, and another runnable when it ends.
 * Useful for running and then stopping a motor, or extending and then retracting a solenoid.
 * Has no end condition as-is; either subclass it or use {@link Command#withTimeout(double)} or
 * {@link Command#interruptOn(BooleanSupplier)} to give it one.
 */
public class StartEndCommand extends SendableCommandBase {

  protected final Runnable m_onInit;
  protected final Runnable m_onEnd;

  /**
   * Creates a new StartEndCommand.  Will run the given runnables when the command starts and when
   * it ends.
   *
   * @param onInit       the Runnable to run on command init
   * @param onEnd        the Runnable to run on command end
   * @param requirements the subsystems required by this command
   */
  public StartEndCommand(Runnable onInit, Runnable onEnd, Subsystem... requirements) {
    m_onInit = onInit;
    m_onEnd = onEnd;
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_onInit.run();
  }

  @Override
  public void end(boolean interrupted) {
    m_onEnd.run();
  }
}
