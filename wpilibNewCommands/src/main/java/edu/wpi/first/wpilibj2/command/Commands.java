// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.units.measure.Time;
import java.util.Map;
import java.util.Set;
import java.util.function.BooleanSupplier;
import java.util.function.Supplier;

/**
 * Namespace for command factory methods.
 *
 * <p>For convenience, you might want to static import the members of this class.
 */
public final class Commands {
  /**
   * Constructs a command that does nothing, finishing immediately.
   *
   * @return the command
   */
  public static Command none() {
    return new InstantCommand();
  }

  /**
   * Constructs a command that does nothing until interrupted.
   *
   * @param requirements Subsystems to require
   * @return the command
   */
  public static Command idle(Subsystem... requirements) {
    return run(() -> {}, requirements);
  }

  // Action Commands

  /**
   * Constructs a command that runs an action once and finishes.
   *
   * @param action the action to run
   * @param requirements subsystems the action requires
   * @return the command
   * @see InstantCommand
   */
  public static Command runOnce(Runnable action, Subsystem... requirements) {
    return new InstantCommand(action, requirements);
  }

  /**
   * Constructs a command that runs an action every iteration until interrupted.
   *
   * @param action the action to run
   * @param requirements subsystems the action requires
   * @return the command
   * @see RunCommand
   */
  public static Command run(Runnable action, Subsystem... requirements) {
    return new RunCommand(action, requirements);
  }

  /**
   * Constructs a command that runs an action once and another action when the command is
   * interrupted.
   *
   * @param start the action to run on start
   * @param end the action to run on interrupt
   * @param requirements subsystems the action requires
   * @return the command
   * @see StartEndCommand
   */
  public static Command startEnd(Runnable start, Runnable end, Subsystem... requirements) {
    return new StartEndCommand(start, end, requirements);
  }

  /**
   * Constructs a command that runs an action every iteration until interrupted, and then runs a
   * second action.
   *
   * @param run the action to run every iteration
   * @param end the action to run on interrupt
   * @param requirements subsystems the action requires
   * @return the command
   */
  public static Command runEnd(Runnable run, Runnable end, Subsystem... requirements) {
    requireNonNullParam(end, "end", "Command.runEnd");
    return new FunctionalCommand(
        () -> {}, run, interrupted -> end.run(), () -> false, requirements);
  }

  /**
   * Constructs a command that runs an action once, and then runs an action every iteration until
   * interrupted.
   *
   * @param start the action to run on start
   * @param run the action to run every iteration
   * @param requirements subsystems the action requires
   * @return the command
   */
  public static Command startRun(Runnable start, Runnable run, Subsystem... requirements) {
    return new FunctionalCommand(start, run, interrupted -> {}, () -> false, requirements);
  }

  /**
   * Constructs a command that prints a message and finishes.
   *
   * @param message the message to print
   * @return the command
   * @see PrintCommand
   */
  public static Command print(String message) {
    return new PrintCommand(message);
  }

  // Idling Commands

  /**
   * Constructs a command that does nothing, finishing after a specified duration.
   *
   * @param seconds after how long the command finishes
   * @return the command
   * @see WaitCommand
   */
  public static Command waitSeconds(double seconds) {
    return new WaitCommand(seconds);
  }

  /**
   * Constructs a command that does nothing, finishing after a specified duration.
   *
   * @param time after how long the command finishes
   * @return the command
   * @see WaitCommand
   */
  public static Command waitTime(Time time) {
    return new WaitCommand(time);
  }

  /**
   * Constructs a command that does nothing, finishing once a condition becomes true.
   *
   * @param condition the condition
   * @return the command
   * @see WaitUntilCommand
   */
  public static Command waitUntil(BooleanSupplier condition) {
    return new WaitUntilCommand(condition);
  }

  // Selector Commands

  /**
   * Runs one of two commands, based on the boolean selector function.
   *
   * @param onTrue the command to run if the selector function returns true
   * @param onFalse the command to run if the selector function returns false
   * @param selector the selector function
   * @return the command
   * @see ConditionalCommand
   */
  public static Command either(Command onTrue, Command onFalse, BooleanSupplier selector) {
    return new ConditionalCommand(onTrue, onFalse, selector);
  }

  /**
   * Runs one of several commands, based on the selector function.
   *
   * @param <K> The type of key used to select the command
   * @param selector the selector function
   * @param commands map of commands to select from
   * @return the command
   * @see SelectCommand
   */
  public static <K> Command select(Map<K, Command> commands, Supplier<? extends K> selector) {
    return new SelectCommand<>(commands, selector);
  }

  /**
   * Runs the command supplied by the supplier.
   *
   * @param supplier the command supplier
   * @param requirements the set of requirements for this command
   * @return the command
   * @see DeferredCommand
   */
  public static Command defer(Supplier<Command> supplier, Set<Subsystem> requirements) {
    return new DeferredCommand(supplier, requirements);
  }

  /**
   * Constructs a command that schedules the command returned from the supplier when initialized,
   * and ends when it is no longer scheduled. The supplier is called when the command is
   * initialized.
   *
   * @param supplier the command supplier
   * @return the command
   * @see ProxyCommand
   * @see DeferredCommand
   */
  public static Command deferredProxy(Supplier<Command> supplier) {
    return defer(() -> supplier.get().asProxy(), Set.of());
  }

  // Command Groups

  /**
   * Runs a group of commands in series, one after the other.
   *
   * @param commands the commands to include
   * @return the command group
   * @see SequentialCommandGroup
   */
  public static Command sequence(Command... commands) {
    return new SequentialCommandGroup(commands);
  }

  /**
   * Runs a group of commands in series, one after the other. Once the last command ends, the group
   * is restarted.
   *
   * @param commands the commands to include
   * @return the command group
   * @see SequentialCommandGroup
   * @see Command#repeatedly()
   */
  public static Command repeatingSequence(Command... commands) {
    return sequence(commands).repeatedly();
  }

  /**
   * Runs a group of commands at the same time. Ends once all commands in the group finish.
   *
   * @param commands the commands to include
   * @return the command
   * @see ParallelCommandGroup
   */
  public static Command parallel(Command... commands) {
    return new ParallelCommandGroup(commands);
  }

  /**
   * Runs a group of commands at the same time. Ends once any command in the group finishes, and
   * cancels the others.
   *
   * @param commands the commands to include
   * @return the command group
   * @see ParallelRaceGroup
   */
  public static Command race(Command... commands) {
    return new ParallelRaceGroup(commands);
  }

  /**
   * Runs a group of commands at the same time. Ends once a specific command finishes, and cancels
   * the others.
   *
   * @param deadline the deadline command
   * @param otherCommands the other commands to include
   * @return the command group
   * @see ParallelDeadlineGroup
   * @throws IllegalArgumentException if the deadline command is also in the otherCommands argument
   */
  public static Command deadline(Command deadline, Command... otherCommands) {
    return new ParallelDeadlineGroup(deadline, otherCommands);
  }

  private Commands() {
    throw new UnsupportedOperationException("This is a utility class");
  }
}
