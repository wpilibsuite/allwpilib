// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import edu.wpi.first.units.measure.Time;
import java.util.List;
import java.util.function.Consumer;
import org.wpilib.annotation.NoDiscard;

/**
 * Generic interface to represent mechanisms on a robot. Commands can require sole ownership of a
 * mechanism; when a command that requires a mechanism is running, no other commands may use it at
 * the same time.
 *
 * <p>Unlike the Subsystem interface of commands v2, Mechanism does not need to be explicitly
 * registered in the constructor.
 *
 * <p>Even though this interface is named "Mechanism", it may be used to represent other physical
 * hardware on a robot that should be controlled with commands - for example, an LED strip or a
 * vision processor that can switch between different pipelines could be represented as mechanisms.
 */
public interface Mechanism {
  /**
   * Gets the scheduler that registered this subsystem.
   *
   * @return The registered scheduler.
   */
  default Scheduler getRegisteredScheduler() {
    return Scheduler.getDefault();
  }

  /**
   * Gets the name of this mechanism.
   *
   * @return The name of the mechanism.
   */
  @NoDiscard
  default String getName() {
    return this.getClass().getSimpleName();
  }

  /**
   * Sets the default command to run on the mechanism when no other command is scheduled. The
   * default command's priority is effectively the minimum allowable priority for any command
   * requiring a mechanism. For this reason, it's recommended that a default command have a priority
   * less than {@link Command#DEFAULT_PRIORITY} so it doesn't prevent low-priority commands from
   * running.
   *
   * <p>The default command is initially an idle command that only owns the mechanism without doing
   * anything. This command has the lowest possible priority to allow any other command to run.
   *
   * @param defaultCommand the new default command
   */
  default void setDefaultCommand(Command defaultCommand) {
    getRegisteredScheduler().setDefaultCommand(this, defaultCommand);
  }

  /**
   * Gets the default command that was set by the latest call to {@link
   * #setDefaultCommand(Command)}.
   *
   * @return The currently configured default command
   */
  default Command getDefaultCommand() {
    return getRegisteredScheduler().getDefaultCommandFor(this);
  }

  /**
   * Starts building a command that requires this mechanism.
   *
   * @param commandBody The main function body of the command.
   * @return The command builder, for further configuration.
   */
  default NeedsNameBuilderStage run(Consumer<Coroutine> commandBody) {
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
  default NeedsNameBuilderStage runRepeatedly(Runnable loopBody) {
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
  default Command idle() {
    return run(Coroutine::park).withPriority(Command.LOWEST_PRIORITY).named(getName() + "[IDLE]");
  }

  /**
   * Returns a command that idles this mechanism for the given duration of time.
   *
   * @param duration How long the mechanism should idle for.
   * @return A new idle command.
   */
  default Command idleFor(Time duration) {
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
  @NoDiscard
  default List<Command> getRunningCommands() {
    return getRegisteredScheduler().getRunningCommandsFor(this);
  }

  /**
   * Creates a dummy mechanism, for use in unit tests.
   *
   * @param name The name of this dummy mechanism.
   * @param scheduler The registered scheduler. Cannot be null.
   * @return The dummy mechanism.
   */
  static Mechanism createDummy(String name, Scheduler scheduler) {
    return new Mechanism() {
      @Override
      public String getName() {
        return name;
      }

      @Override
      public Scheduler getRegisteredScheduler() {
        return scheduler;
      }
    };
  }
}
