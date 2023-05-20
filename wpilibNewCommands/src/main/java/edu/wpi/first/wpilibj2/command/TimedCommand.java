// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.wpilibj.Timer;
import java.util.function.DoubleConsumer;

/**
 * A command that runs a {@link DoubleConsumer} continuously with elapsed time since the command has
 * been initialized as the input. Has no end condition as-is; either subclass it or use {@link
 * Command#withTimeout(double)} or {@link Command#until(BooleanSupplier)} to give it one.
 *
 * <p>This is essentially {@link RunCommand} with a DoubleConsumer for time.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
public class TimedCommand extends CommandBase {
  private final Timer m_timer = new Timer();
  private final DoubleConsumer m_output;

  /**
   * Creates a new TimedCommand. The DoubleConsumer will be run continuously with time elapsed since
   * initialization until the command ends.
   *
   * @param toRun the DoubleConsumer to run based on elapsed time
   * @param requirements the subsystems to require
   */
  public TimedCommand(DoubleConsumer toRun, Subsystem... requirements) {
    m_output = requireNonNullParam(toRun, "toRun", "TimedCommand");
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_timer.restart();
  }

  @Override
  public void execute() {
    m_output.accept(m_timer.get());
  }

  @Override
  public void end(boolean interrupted) {
    m_timer.stop();
  }
}
