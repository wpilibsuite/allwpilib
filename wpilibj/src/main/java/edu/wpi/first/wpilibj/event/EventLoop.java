// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import java.util.Collection;
import java.util.LinkedHashSet;
import java.util.function.BooleanSupplier;

/** The loop polling {@link BooleanEvent} objects and executing the actions bound to them. */
public final class EventLoop {
  private final Collection<Binding> m_bindings = new LinkedHashSet<>();

  /**
   * Bind a new action to run whenever the condition is true.
   *
   * @param condition the condition to listen to.
   * @param action the action to run.
   */
  public void bind(BooleanSupplier condition, Runnable action) {
    m_bindings.add(new Binding(condition, action));
  }

  /** Poll all bindings. */
  public void poll() {
    m_bindings.forEach(Binding::poll);
  }

  /** Clear all bindings. */
  public void clear() {
    m_bindings.clear();
  }

  private static class Binding {
    private final BooleanSupplier m_condition;
    private final Runnable m_action;

    private Binding(BooleanSupplier condition, Runnable action) {
      this.m_condition = condition;
      this.m_action = action;
    }

    void poll() {
      if (m_condition.getAsBoolean()) {
        m_action.run();
      }
    }
  }
}
