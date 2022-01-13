// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/**
 * A command that allows the user to pass in functions for each of the basic command methods through
 * the constructor. Useful for inline definitions of complex commands - note, however, that if a
 * command is beyond a certain complexity it is usually better practice to write a proper class for
 * it than to inline it.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class FunctionalCommand extends CommandBase {
  protected final Runnable m_onInit;
  protected final Runnable m_onExecute;
  protected final Consumer<Boolean> m_onEnd;
  protected final BooleanSupplier m_isFinished;

  /**
   * Creates a new FunctionalCommand.
   *
   * @param onInit the function to run on command initialization
   * @param onExecute the function to run on command execution
   * @param onEnd the function to run on command end
   * @param isFinished the function that determines whether the command has finished
   * @param requirements the subsystems required by this command
   */
  public FunctionalCommand(
      Runnable onInit,
      Runnable onExecute,
      Consumer<Boolean> onEnd,
      BooleanSupplier isFinished,
      Subsystem... requirements) {
    m_onInit = requireNonNullParam(onInit, "onInit", "FunctionalCommand");
    m_onExecute = requireNonNullParam(onExecute, "onExecute", "FunctionalCommand");
    m_onEnd = requireNonNullParam(onEnd, "onEnd", "FunctionalCommand");
    m_isFinished = requireNonNullParam(isFinished, "isFinished", "FunctionalCommand");

    addRequirements(requirements);
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
