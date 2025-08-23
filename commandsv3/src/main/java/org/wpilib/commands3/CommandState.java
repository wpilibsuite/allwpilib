// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

/** Represents the state of a command as it is executed by the scheduler. */
final class CommandState {
  // Two billion unique IDs should be enough for anybody.
  @SuppressWarnings("PMD.RedundantFieldInitializer") // we're explicitly counting up from zero
  private static int s_lastId = 0;

  private final Command m_command;
  private final Command m_parent;
  private final Coroutine m_coroutine;
  private final Binding m_binding;
  private double m_lastRuntimeMs = -1;
  private double m_totalRuntimeMs;
  private final int m_id;

  /**
   * Creates a new command state object.
   *
   * @param command The command being tracked.
   * @param parent The parent command composition that scheduled the tracked command. Null if the
   *     command was scheduled by a top level construct like trigger bindings or manually scheduled
   *     by a user. For deeply nested compositions, this tracks the <i>direct parent command</i>
   *     that invoked the schedule() call; in this manner, an ancestry tree can be built, where each
   *     {@code CommandState} object references a parent node in the tree.
   * @param coroutine The coroutine to which the command is bound.
   * @param binding The binding that caused the command to be scheduled.
   */
  CommandState(Command command, Command parent, Coroutine coroutine, Binding binding) {
    m_command = command;
    m_parent = parent;
    m_coroutine = coroutine;
    m_binding = binding;

    // This is incredibly non-thread safe, but nobody should be using the command framework across
    // multiple threads anyway. Worst case scenario, we'll get duplicate IDs for commands scheduled
    // by different threads and cause bad telemetry data without affecting program correctness.
    m_id = ++s_lastId;
  }

  public Command command() {
    return m_command;
  }

  public Command parent() {
    return m_parent;
  }

  public Coroutine coroutine() {
    return m_coroutine;
  }

  // may return null
  public Binding binding() {
    return m_binding;
  }

  /**
   * How long the command took to run the last time it executed. Defaults to -1 if the command has
   * not run at least once.
   *
   * @return The runtime, in milliseconds.
   */
  public double lastRuntimeMs() {
    return m_lastRuntimeMs;
  }

  public void setLastRuntimeMs(double lastRuntimeMs) {
    m_lastRuntimeMs = lastRuntimeMs;
    m_totalRuntimeMs += lastRuntimeMs;
  }

  public double totalRuntimeMs() {
    return m_totalRuntimeMs;
  }

  public int id() {
    return m_id;
  }

  @Override
  public boolean equals(Object obj) {
    return obj instanceof CommandState that && m_id == that.m_id;
  }

  @Override
  public int hashCode() {
    return m_id;
  }

  @Override
  public String toString() {
    return "CommandState[command=%s, parent=%s, coroutine=%s, id=%d]"
        .formatted(m_command, m_parent, m_coroutine, m_id);
  }
}
