// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import edu.wpi.first.units.measure.Time;
import java.util.List;
import java.util.function.Consumer;

/**
 * Generic base class to represent mechanisms on a robot. Commands can require sole ownership of a
 * mechanism; when a command that requires a mechanism is running, no other commands may use it at
 * the same time.
 *
 * <p>Even though this class is named "Mechanism", it may be used to represent other physical
 * hardware on a robot that should be controlled with commands - for example, an LED strip or a
 * vision processor that can switch between different pipelines could be represented as mechanisms.
 */
public class Mechanism {
  private final String m_name;

  private final Scheduler m_registeredScheduler;

  /**
   * Creates a new mechanism registered with the default scheduler instance and named using the name
   * of the class. Intended to be used by subclasses to get sane defaults without needing to
   * manually declare a constructor.
   */
  @SuppressWarnings("this-escape")
  protected Mechanism() {
    m_name = getClass().getSimpleName();
    m_registeredScheduler = Scheduler.getDefault();
    setDefaultCommand(idle());
  }

  /**
   * Creates a new mechanism, registered with the default scheduler instance.
   *
   * @param name The name of the mechanism. Cannot be null.
   */
  public Mechanism(String name) {
    this(name, Scheduler.getDefault());
  }

  /**
   * Creates a new mechanism, registered with the given scheduler instance.
   *
   * @param name The name of the mechanism. Cannot be null.
   * @param scheduler The registered scheduler. Cannot be null.
   */
  @SuppressWarnings("this-escape")
  public Mechanism(String name, Scheduler scheduler) {
    m_name = name;
    m_registeredScheduler = scheduler;
    setDefaultCommand(idle());
  }

  /**
   * Gets the name of this mechanism.
   *
   * @return The name of the mechanism.
   */
  public String getName() {
    return m_name;
  }

  /**
   * Sets the default command to run on the mechanism when no other command is scheduled. The
   * default command's priority is effectively the minimum allowable priority for any command
   * requiring a mechanism. For this reason, it's recommended that a default command have a priority
   * less than{@link Command#DEFAULT_PRIORITY} to prevent it from blocking other, non-default
   * commands from running.
   *
   * <p>The default command is initially an idle command that only owns the mechanism without doing
   * anything. This command has the lowest possible priority to allow any other command to run.
   *
   * @param defaultCommand the new default command
   */
  public void setDefaultCommand(Command defaultCommand) {
    m_registeredScheduler.scheduleAsDefaultCommand(this, defaultCommand);
  }

  /**
   * Gets the default command that was set by the latest call to {@link
   * #setDefaultCommand(Command)}.
   *
   * @return The currently configured default command
   */
  public Command getDefaultCommand() {
    return m_registeredScheduler.getDefaultCommandFor(this);
  }

  /**
   * Starts building a command that requires this mechanism.
   *
   * @param commandBody The main function body of the command.
   * @return The command builder, for further configuration.
   */
  public NeedsNameBuilderStage run(Consumer<Coroutine> commandBody) {
    return new StagedCommandBuilder().requiring(this).executing(commandBody);
  }

  /**
   * Starts building a command that requires this mechanism. The given function will be called
   * repeatedly in an infinite loop. Useful for building commands that don't need state or multiple
   * stages of logic.
   *
   * @param loopBody The body of the infinite loop.
   * @return The command builder, for further configuration.
   */
  public NeedsNameBuilderStage runRepeatedly(Runnable loopBody) {
    return run(
        coroutine -> {
          while (true) {
            loopBody.run();
            coroutine.yield();
          }
        });
  }

  /**
   * Returns a command that idles this mechanism until another command claims it. The idle command
   * has {@link Command#LOWEST_PRIORITY the lowest priority} and can be interrupted by any other
   * command.
   *
   * <p>The {@link #getDefaultCommand() default command} for every mechanism is an idle command
   * unless a different default command has been configured.
   *
   * @return A new idle command.
   */
  public Command idle() {
    return new IdleCommand(this);
  }

  /**
   * Returns a command that idles this mechanism for the given duration of time.
   *
   * @param duration How long the mechanism should idle for.
   * @return A new idle command.
   */
  public Command idle(Time duration) {
    return idle().withTimeout(duration);
  }

  /**
   * Gets all running commands that require this mechanism. Commands are returned in the order in
   * which they were scheduled. The returned list is read-only. Every command in the list will have
   * been scheduled by the previous entry in the list or by intermediate commands that do not
   * require the mechanism.
   *
   * @return The currently running commands that require the mechanism.
   */
  public List<Command> getRunningCommands() {
    return m_registeredScheduler.getRunningCommandsFor(this);
  }

  @Override
  public String toString() {
    return m_name;
  }
}
