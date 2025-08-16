// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import edu.wpi.first.wpilibj.RobotController;
import java.util.function.Consumer;

/**
 * An event that occurs during scheduler processing. This can range from {@link Scheduled a command
 * being scheduled} by a trigger or manual call to {@link Scheduler#schedule(Command)} to {@link
 * Interrupted a command being interrupted by another}. Event listeners can be registered with a
 * scheduler via {@link Scheduler#addEventListener(Consumer)}. All events have a timestamp to
 * indicate when the event occurred.
 */
public sealed interface SchedulerEvent {
  /**
   * The timestamp for when the event occurred. Measured in microseconds since some arbitrary start
   * time.
   *
   * @return The event timestamp.
   * @see RobotController#getTime()
   */
  long timestampMicros();

  static SchedulerEvent scheduled(Command command) {
    return new Scheduled(command, RobotController.getTime());
  }

  static SchedulerEvent mounted(Command command) {
    return new Mounted(command, RobotController.getTime());
  }

  static SchedulerEvent yielded(Command command) {
    return new Yielded(command, RobotController.getTime());
  }

  static SchedulerEvent completed(Command command) {
    return new Completed(command, RobotController.getTime());
  }

  static SchedulerEvent completedWithError(Command command, Throwable error) {
    return new CompletedWithError(command, error, RobotController.getTime());
  }

  static SchedulerEvent evicted(Command command) {
    return new Evicted(command, RobotController.getTime());
  }

  static SchedulerEvent interrupted(Command command, Command interrupter) {
    return new Interrupted(command, interrupter, RobotController.getTime());
  }

  /**
   * An event marking when a command is scheduled in {@link Scheduler#schedule(Command)}.
   *
   * @param command The command that was scheduled
   * @param timestampMicros When the command was scheduled
   */
  record Scheduled(Command command, long timestampMicros) implements SchedulerEvent {}

  /**
   * An event marking when a command starts running.
   *
   * @param command The command that started
   * @param timestampMicros When the command started
   */
  record Mounted(Command command, long timestampMicros) implements SchedulerEvent {}

  /**
   * An event marking when a command yielded control with {@link Coroutine#yield()}.
   *
   * @param command The command that yielded
   * @param timestampMicros When the command yielded
   */
  record Yielded(Command command, long timestampMicros) implements SchedulerEvent {}

  /**
   * An event marking when a command completed naturally.
   *
   * @param command The command that completed
   * @param timestampMicros When the command completed
   */
  record Completed(Command command, long timestampMicros) implements SchedulerEvent {}

  /**
   * An event marking when a command threw or encountered an unhanded exception.
   *
   * @param command The command that encountered the error
   * @param error The unhandled error
   * @param timestampMicros When the error occurred
   */
  record CompletedWithError(Command command, Throwable error, long timestampMicros)
      implements SchedulerEvent {}

  /**
   * An event marking when a command was evicted from the scheduler. Commands may be evicted when
   * they've been scheduled, then another command requiring a shared mechanism is scheduled
   * afterward; when cancelled via {@link Scheduler#cancel(Command)} or {@link
   * Scheduler#cancelAll()}; or when they're running and interrupted by another command requiring a
   * shared mechanism.
   *
   * @param command The command that was evicted
   * @param timestampMicros When the command was evicted
   */
  record Evicted(Command command, long timestampMicros) implements SchedulerEvent {}

  /**
   * An event marking when a command was interrupted by another. Typically followed by an {@link
   * Evicted} event.
   *
   * @param command The command that was interrupted
   * @param interrupter The interrupting command
   * @param timestampMicros When the command was interrupted
   */
  record Interrupted(Command command, Command interrupter, long timestampMicros)
      implements SchedulerEvent {}
}
