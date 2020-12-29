// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import java.util.function.BooleanSupplier;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A command that runs a given runnable when it is initialized, and another runnable when it ends.
 * Useful for running and then stopping a motor, or extending and then retracting a solenoid.
 * Has no end condition as-is; either subclass it or use {@link Command#withTimeout(double)} or
 * {@link Command#withInterrupt(BooleanSupplier)} to give it one.
 */
public class StartEndCommand extends CommandBase {
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
    m_onInit = requireNonNullParam(onInit, "onInit", "StartEndCommand");
    m_onEnd = requireNonNullParam(onEnd, "onEnd", "StartEndCommand");

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
