// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import java.util.Collection;
import java.util.LinkedHashSet;

/**
 * A declarative way to bind a set of actions to a loop and execute them when the loop is polled.
 */
public final class EventLoop {
  private final Collection<Runnable> m_bindings = new LinkedHashSet<>();

  /**
   * Bind a new action to run when the loop is polled.
   *
   * @param action the action to run.
   */
  public void bind(Runnable action) {
    m_bindings.add(action);
  }

  /** Poll all bindings. */
  public void poll() {
    m_bindings.forEach(Runnable::run);
  }

  /** Clear all bindings. */
  public void clear() {
    m_bindings.clear();
  }
}
