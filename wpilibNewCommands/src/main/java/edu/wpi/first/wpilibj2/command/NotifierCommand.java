/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.util.function.BooleanSupplier;

import edu.wpi.first.wpilibj.Notifier;

/**
 * A command that starts a notifier to run the given runnable periodically in a separate thread.
 * Has no end condition as-is; either subclass it or use {@link Command#withTimeout(double)} or
 * {@link Command#withInterrupt(BooleanSupplier)} to give it one.
 *
 * <p>WARNING: Do not use this class unless you are confident in your ability to make the executed
 * code thread-safe.  If you do not know what "thread-safe" means, that is a good sign that
 * you should not use this class.
 */
public class NotifierCommand extends CommandBase {
  protected final Notifier m_notifier;
  protected final double m_period;

  /**
   * Creates a new NotifierCommand.
   *
   * @param toRun        the runnable for the notifier to run
   * @param period       the period at which the notifier should run, in seconds
   * @param requirements the subsystems required by this command
   */
  public NotifierCommand(Runnable toRun, double period, Subsystem... requirements) {
    m_notifier = new Notifier(toRun);
    m_period = period;
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_notifier.startPeriodic(m_period);
  }

  @Override
  public void end(boolean interrupted) {
    m_notifier.stop();
  }
}
