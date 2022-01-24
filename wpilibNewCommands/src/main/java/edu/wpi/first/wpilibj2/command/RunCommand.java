// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.util.function.BooleanSupplier;

/**
 * A command that runs a Runnable continuously. Has no end condition as-is; either subclass it or
 * use {@link Command#withTimeout(double)} or {@link Command#withInterrupt(BooleanSupplier)} to give
 * it one. If you only wish to execute a Runnable once, use {@link InstantCommand}.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class RunCommand extends CommandBase {
  protected final Runnable m_toRun;

  /**
   * Creates a new RunCommand. The Runnable will be run continuously until the command ends. Does
   * not run when disabled.
   *
   * @param toRun the Runnable to run
   * @param requirements the subsystems to require
   */
  public RunCommand(Runnable toRun, Subsystem... requirements) {
    m_toRun = requireNonNullParam(toRun, "toRun", "RunCommand");
    addRequirements(requirements);
  }

  @Override
  public void execute() {
    m_toRun.run();
  }
}
