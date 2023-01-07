// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import java.util.Collection;
import java.util.LinkedHashSet;

/** The loop polling {@link BooleanEvent} objects and executing the actions bound to them. */
public final class EventLoop {
  private final Collection<Runnable> m_bindings = new LinkedHashSet<>();

  /**
   * Bind a new action to run whenever the condition is true.
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
