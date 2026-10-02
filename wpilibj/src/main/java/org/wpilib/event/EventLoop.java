// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.event;

import java.util.ConcurrentModificationException;
import java.util.LinkedHashSet;
import org.wpilib.util.container.WeakLinkedHashSet;

/**
 * A declarative way to bind a set of actions to a loop and execute them when the loop is polled.
 */
public final class EventLoop {
  // Keep all bindings in one insertion-ordered collection for polling.
  private final WeakLinkedHashSet<Runnable> m_bindings = new WeakLinkedHashSet<>();
  // Keep strong references for strongly bound actions to prevent garbage collection.
  private final LinkedHashSet<Runnable> m_strongBindings = new LinkedHashSet<>();
  private boolean m_running;

  /** Default constructor. */
  public EventLoop() {}

  /**
   * Bind a new action to run when the loop is polled. The event loop keeps a reference to the
   * action, which will prevent it from being garbage collected. If memory leaks are a concern,
   * consider {@link #bindWeak(Runnable)}.
   *
   * @param action the action to run.
   */
  public void bind(Runnable action) {
    if (m_running) {
      throw new ConcurrentModificationException("Cannot bind EventLoop while it is running");
    }
    m_bindings.add(action);
    m_strongBindings.add(action);
  }

  /**
   * Weakly binds a new action to run when the loop is polled. Unlike {@link #bind(Runnable)}, a
   * weakly bound action will not be prevented from being garbage collected.
   *
   * @param action the action to run
   */
  public void bindWeak(Runnable action) {
    if (m_running) {
      throw new ConcurrentModificationException("Cannot bind EventLoop while it is running");
    }
    m_bindings.add(action);
  }

  /**
   * Checks if an action is bound to the event loop.
   *
   * @param action the action to check
   * @return true if the action is bound, false if not
   */
  public boolean isBound(Runnable action) {
    return m_bindings.contains(action);
  }

  /**
   * Unbind an action from running when the loop is polled. Has no effect if the given action is not
   * already bound.
   *
   * @param action the action to unbind.
   */
  public void unbind(Runnable action) {
    if (m_running) {
      throw new ConcurrentModificationException("Cannot unbind EventLoop while it is running");
    }
    m_bindings.remove(action);
    m_strongBindings.remove(action);
  }

  /** Poll all bindings. */
  @SuppressWarnings("PMD.UnusedAssignment")
  public void poll() {
    try {
      m_running = true;
      m_bindings.forEach(Runnable::run);
    } finally {
      m_running = false;
    }
  }

  /** Clear all bindings. */
  public void clear() {
    if (m_running) {
      throw new ConcurrentModificationException("Cannot clear EventLoop while it is running");
    }
    m_bindings.clear();
    m_strongBindings.clear();
  }
}
