// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

/** Represents the state of a command as it is executed by the scheduler. */
final class CommandState {
  private final Command command;
  private final Command parent;
  private final Coroutine coroutine;
  private final Binding binding; // may be null
  private double lastRuntimeMs = -1;
  private double totalRuntimeMs = 0;
  private final int id = System.identityHashCode(this);

  /**
   * @param command The command being tracked.
   * @param parent The parent command composition that scheduled the tracked command. Null if the
   *     command was scheduled by a top level construct like trigger bindings or manually scheduled
   *     by a user. For deeply nested compositions, this tracks the <i>direct parent command</i>
   *     that invoked the schedule() call; in this manner, an ancestry tree can be built, where each
   *     {@code CommandState} object references a parent node in the tree.
   * @param coroutine The coroutine to which the command is bound.
   * @param scheduleFrames The stack frames for the schedule site; that is, the backtrace of the
   *     code that caused the command to be scheduled. These let us report better traces when
   *     commands encounter exceptions.
   */
  CommandState(Command command, Command parent, Coroutine coroutine, Binding binding) {
    this.command = command;
    this.parent = parent;
    this.coroutine = coroutine;
    this.binding = binding; // may be null
  }

  public Command command() {
    return command;
  }

  public Command parent() {
    return parent;
  }

  public Coroutine coroutine() {
    return coroutine;
  }

  // may return null
  public Binding binding() {
    return binding;
  }

  /**
   * How long the command took to run the last time it executed. Defaults to -1 if the command has
   * not run at least once.
   *
   * @return The runtime, in milliseconds.
   */
  public double lastRuntimeMs() {
    return lastRuntimeMs;
  }

  public void setLastRuntimeMs(double lastRuntimeMs) {
    this.lastRuntimeMs = lastRuntimeMs;
    totalRuntimeMs += lastRuntimeMs;
  }

  public double totalRuntimeMs() {
    return totalRuntimeMs;
  }

  public int id() {
    return id;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof CommandState that && this.id == that.id;
  }

  @Override
  public int hashCode() {
    return id;
  }

  @Override
  public String toString() {
    return "CommandState["
        + "command="
        + command
        + ", "
        + "parent="
        + parent
        + ", "
        + "coroutine="
        + coroutine
        + ']';
  }
}
