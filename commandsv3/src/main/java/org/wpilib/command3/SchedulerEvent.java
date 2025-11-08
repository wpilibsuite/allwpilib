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
   * An event marking when a command was canceled. If the command was canceled because it was
   * interrupted by another command, an {@link Interrupted} will be emitted immediately prior to the
   * cancellation event.
   *
   * @param command The command that was canceled
   * @param timestampMicros When the command was canceled
   */
  record Canceled(Command command, long timestampMicros) implements SchedulerEvent {}

  /**
   * An event marking when a command was interrupted by another. Typically followed by an {@link
   * Canceled} event.
   *
   * @param command The command that was interrupted
   * @param interrupter The interrupting command
   * @param timestampMicros When the command was interrupted
   */
  record Interrupted(Command command, Command interrupter, long timestampMicros)
      implements SchedulerEvent {}
}
