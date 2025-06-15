// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import edu.wpi.first.units.measure.Time;
import java.util.List;
import java.util.function.Consumer;

/**
 * A resource that may be claimed by a command. A single claimable resource cannot be claimed by
 * more than one running command at a time.
 */
public class RequireableResource {
  private final String m_name;

  private final Scheduler m_registeredScheduler;

  /**
   * Creates a new requireable resource registered with the default scheduler instance and named
   * using the name of the class. Intended to be used by subclasses to get sane defaults without
   * needing to manually declare a constructor.
   */
  @SuppressWarnings("this-escape")
  protected RequireableResource() {
    m_name = getClass().getSimpleName();
    m_registeredScheduler = Scheduler.getDefault();
    setDefaultCommand(idle());
  }

  /**
   * Creates a new claimable resource, registered with the default scheduler instance.
   *
   * @param name The name of the resource. Cannot be null.
   */
  public RequireableResource(String name) {
    this(name, Scheduler.getDefault());
  }

  /**
   * Creates a new claimable resource, registered with the given scheduler instance.
   *
   * @param name The name of the resource. Cannot be null.
   * @param scheduler The registered scheduler. Cannot be null.
   */
  @SuppressWarnings("this-escape")
  public RequireableResource(String name, Scheduler scheduler) {
    m_name = name;
    m_registeredScheduler = scheduler;
    setDefaultCommand(idle());
  }

  public String getName() {
    return m_name;
  }

  /**
   * Sets the default command to run on the resource when no other command is scheduled. The default
   * command's priority is effectively the minimum allowable priority for any command requiring a
   * resource. For this reason, it's required that a default command have a priority less than
   * {@link Command#DEFAULT_PRIORITY} to prevent it from blocking other, non-default commands from
   * running.
   *
   * <p>The default command is initially an idle command that merely parks the execution thread.
   * This command has the lowest possible priority so as to allow any other command to run.
   *
   * @param defaultCommand the new default command
   */
  public void setDefaultCommand(Command defaultCommand) {
    m_registeredScheduler.scheduleAsDefaultCommand(this, defaultCommand);
  }

  /**
   * Gets the default command that was set by the latest call to
   * {@link #setDefaultCommand(Command)}.
   *
   * @return The currently configured default command
   */
  public Command getDefaultCommand() {
    return m_registeredScheduler.getDefaultCommandFor(this);
  }

  /**
   * Starts building a command that requires this resource.
   *
   * @param commandBody The main function body of the command.
   * @return The command builder, for further configuration.
   */
  public CommandBuilder run(Consumer<Coroutine> commandBody) {
    return new CommandBuilder().requiring(this).executing(commandBody);
  }

  /**
   * Starts building a command that requires this resource. The given function will be called
   * repeatedly in an infinite loop. Useful for building commands that don't need state or multiple
   * stages of logic.
   *
   * @param loopBody The body of the infinite loop.
   * @return The command builder, for further configuration.
   */
  public CommandBuilder runRepeatedly(Runnable loopBody) {
    return run(coroutine -> {
      while (true) {
        loopBody.run();
        coroutine.yield();
      }
    });
  }

  /**
   * Returns a command that idles this resource until another command claims it. The idle command
   * has {@link Command#LOWEST_PRIORITY the lowest priority} and can be interrupted by any other
   * command.
   *
   * <p>The default command for every claimable resource is an idle command.</p>
   * @return A new idle command.
   */
  public Command idle() {
    return new IdleCommand(this);
  }

  /**
   * Returns a command that idles this resource for the given duration of time.
   * @param duration How long the resource should idle for.
   * @return A new idle command.
   */
  public Command idle(Time duration) {
    return idle().withTimeout(duration);
  }

  public List<Command> getRunningCommands() {
    return m_registeredScheduler.getRunningCommandsFor(this);
  }

  @Override
  public String toString() {
    return m_name;
  }
}
