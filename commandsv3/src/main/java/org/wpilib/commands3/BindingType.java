// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

/** Describes when a command bound to a trigger should run. */
enum BindingType {
  /**
   * An immediate or manual binding created when calling {@link Scheduler#schedule(Command)}
   * directly, without using a trigger to bind it to a signal.
   */
  IMMEDIATE,
  /**
   * Schedules (forks) a command on a rising edge signal. The command will run until it completes or
   * is interrupted by another command requiring the same resources.
   */
  SCHEDULE_ON_RISING_EDGE,
  /**
   * Schedules (forks) a command on a falling edge signal. The command will run until it completes
   * or is interrupted by another command requiring the same resources.
   */
  SCHEDULE_ON_FALLING_EDGE,
  /**
   * Schedules (forks) a command on a rising edge signal. If the command is still running on the
   * next rising edge, it will be cancelled then; otherwise, it will be scheduled again.
   */
  TOGGLE_ON_RISING_EDGE,
  /**
   * Schedules (forks) a command on a falling edge signal. If the command is still running on the
   * next falling edge, it will be cancelled then; otherwise, it will be scheduled again.
   */
  TOGGLE_ON_FALLING_EDGE,
  /**
   * Schedules a command on a rising edge signal. If the command is still running on the next
   * falling edge, it will be cancelled then - unlike {@link #SCHEDULE_ON_RISING_EDGE}, which would
   * allow it to continue to run.
   */
  RUN_WHILE_HIGH,
  /**
   * Schedules a command on a falling edge signal. If the command is still running on the next
   * rising edge, it will be cancelled then - unlike {@link #SCHEDULE_ON_FALLING_EDGE}, which would
   * allow it to continue to run.
   */
  RUN_WHILE_LOW
}
