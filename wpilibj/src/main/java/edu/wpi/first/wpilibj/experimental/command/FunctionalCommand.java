package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

import static java.util.Objects.requireNonNull;

/**
 * A command that allows the user to pass in functions for each of the basic command methods through
 * the constructor.  Useful for inline definitions of complex commands - note, however, that if a
 * command is beyond a certain complexity it is usually better practice to write a proper class for
 * it than to inline it.
 */
public class FunctionalCommand extends SendableCommandBase {

  protected final Runnable m_onInit;
  protected final Runnable m_onExecute;
  protected final Consumer<Boolean> m_onEnd;
  protected final BooleanSupplier m_isFinished;

  /**
   * Creates a new FunctionalCommand.
   *
   * @param onInit     the function to run on command initialization
   * @param onExecute  the function to run on command execution
   * @param onEnd      the function to run on command end
   * @param isFinished the function that determines whether the command has finished
   */
  public FunctionalCommand(Runnable onInit,
                           Runnable onExecute,
                           Consumer<Boolean> onEnd,
                           BooleanSupplier isFinished) {
    requireNonNull(onInit);
    requireNonNull(onExecute);
    requireNonNull(onEnd);
    requireNonNull(isFinished);

    m_onInit = onInit;
    m_onExecute = onExecute;
    m_onEnd = onEnd;
    m_isFinished = isFinished;
  }

  @Override
  public void initialize() {
    m_onInit.run();
  }

  @Override
  public void execute() {
    m_onExecute.run();
  }

  @Override
  public void end(boolean interrupted) {
    m_onEnd.accept(interrupted);
  }

  @Override
  public boolean isFinished() {
    return m_isFinished.getAsBoolean();
  }
}
