// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.cleanup;

import edu.wpi.first.util.ErrorMessages;
import java.util.ArrayDeque;
import java.util.Deque;

/**
 * An object containing a Stack of AutoCloseable objects that are closed when this object is closed.
 */
public class CleanupPool implements AutoCloseable {
  // Use a Deque instead of a Stack, as Stack's iterators go the wrong way, and docs
  // state ArrayDeque is faster anyway.
  private final Deque<AutoCloseable> m_closers = new ArrayDeque<>();

  /** Default constructor. */
  public CleanupPool() {}

  /**
   * Registers an object in the object stack for cleanup.
   *
   * @param <T> The object type
   * @param object The object to register
   * @return The registered object
   */
  public <T extends AutoCloseable> T register(T object) {
    ErrorMessages.requireNonNullParam(object, "object", "register");
    m_closers.addFirst(object);
    return object;
  }

  /**
   * Removes an object from the cleanup stack.
   *
   * @param object the object to remove
   */
  public void remove(AutoCloseable object) {
    m_closers.remove(object);
  }

  /** Closes all objects in the stack. */
  @Override
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  public void close() {
    for (AutoCloseable autoCloseable : m_closers) {
      try {
        autoCloseable.close();
      } catch (Exception e) {
        // Swallow any exceptions on close
        e.printStackTrace();
      }
    }
    m_closers.clear();
  }
}
