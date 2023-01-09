package edu.wpi.first.util.cleanup;

import java.util.Stack;

import edu.wpi.first.util.ErrorMessages;

/**
 * An object containing a Stack of AutoCloseable objects that are closed
 * when this object is closed.
 */
public class CleanupPool implements AutoCloseable {

  private final Stack<AutoCloseable> m_closers = new Stack<AutoCloseable>();

  /**
   * Registers an object in the object stack for cleanup.
   *
   * @param <T> The object type
   * @param object The object to register
   * @return The registered object
   */
  public <T extends AutoCloseable> T register(T object) {
    ErrorMessages.requireNonNullParam(object, "object", "register");
    m_closers.add(object);
    return object;
  }

  /**
   * Removes
   * @param object
   */
  public void remove(AutoCloseable object) {
    m_closers.remove(object);
  }

  /**
   * Closes all objects in the stack.
   */
  @Override
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
