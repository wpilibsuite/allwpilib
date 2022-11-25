// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import java.util.Map;
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
  public static CommandBase none() {
    return new InstantCommand();
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
  public static CommandBase runOnce(Runnable action, Subsystem... requirements) {
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
  public static CommandBase run(Runnable action, Subsystem... requirements) {
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
  public static CommandBase startEnd(Runnable start, Runnable end, Subsystem... requirements) {
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
  public static CommandBase runEnd(Runnable run, Runnable end, Subsystem... requirements) {
    requireNonNullParam(end, "end", "Command.runEnd");
    return new FunctionalCommand(
        () -> {}, run, interrupted -> end.run(), () -> false, requirements);
  }

  /**
   * Constructs a command that prints a message and finishes.
   *
   * @param message the message to print
   * @return the command
   * @see PrintCommand
   */
  public static CommandBase print(String message) {
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
  public static CommandBase waitSeconds(double seconds) {
    return new WaitCommand(seconds);
  }

  /**
   * Constructs a command that does nothing, finishing once a command becomes true.
   *
   * @param condition the condition
   * @return the command
   * @see WaitUntilCommand
   */
  public static CommandBase waitUntil(BooleanSupplier condition) {
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
  public static CommandBase either(Command onTrue, Command onFalse, BooleanSupplier selector) {
    return new ConditionalCommand(onTrue, onFalse, selector);
  }

  /**
   * Runs one of several commands, based on the selector function.
   *
   * @param selector the selector function
   * @param commands map of commands to select from
   * @return the command
   * @see SelectCommand
   */
  public static CommandBase select(Map<Object, Command> commands, Supplier<Object> selector) {
    return new SelectCommand(commands, selector);
  }

  /**
   * Runs a group of commands in series, one after the other.
   *
   * @param commands the commands to include
   * @return the command group
   * @see SequentialCommandGroup
   */
  public static CommandBase sequence(Command... commands) {
    return new SequentialCommandGroup(commands);
  }

  // Command Groups

  /**
   * Runs a group of commands in series, one after the other. Once the last command ends, the group
   * is restarted.
   *
   * @param commands the commands to include
   * @return the command group
   * @see SequentialCommandGroup
   * @see Command#repeatedly()
   */
  public static CommandBase repeatingSequence(Command... commands) {
    return sequence(commands).repeatedly();
  }

  /**
   * Runs a group of commands at the same time. Ends once all commands in the group finish.
   *
   * @param commands the commands to include
   * @return the command
   * @see ParallelCommandGroup
   */
  public static CommandBase parallel(Command... commands) {
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
  public static CommandBase race(Command... commands) {
    return new ParallelRaceGroup(commands);
  }

  /**
   * Runs a group of commands at the same time. Ends once a specific command finishes, and cancels
   * the others.
   *
   * @param deadline the deadline command
   * @param commands the commands to include
   * @return the command group
   * @see ParallelDeadlineGroup
   */
  public static CommandBase deadline(Command deadline, Command... commands) {
    return new ParallelDeadlineGroup(deadline, commands);
  }

  private Commands() {
    throw new UnsupportedOperationException("This is a utility class");
  }
}
