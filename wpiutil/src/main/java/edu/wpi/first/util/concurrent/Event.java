// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.concurrent;

import edu.wpi.first.util.WPIUtilJNI;

/**
 * An atomic signaling event for synchronization.
 *
 * <p>Events have binary state (signaled or not signaled) and may be either automatically reset or
 * manually reset. Automatic-reset events go to non-signaled state when a waitForObject is woken up
 * by the event; manual-reset events require reset() to be called to set the event to non-signaled
 * state; if reset() is not called, any waiter on that event will immediately wake when called.
 */
public final class Event implements AutoCloseable {
  /**
   * Constructor.
   *
   * @param manualReset true for manual reset, false for automatic reset
   * @param initialState true to make the event initially in signaled state
   */
  public Event(boolean manualReset, boolean initialState) {
    m_handle = WPIUtilJNI.createEvent(manualReset, initialState);
  }

  /**
   * Constructor. Initial state is false.
   *
   * @param manualReset true for manual reset, false for automatic reset
   */
  public Event(boolean manualReset) {
    this(manualReset, false);
  }

  /** Constructor. Automatic reset, initial state is false. */
  public Event() {
    this(false, false);
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      WPIUtilJNI.destroyEvent(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Gets the event handle (e.g. for waitForObject).
   *
   * @return handle
   */
  public int getHandle() {
    return m_handle;
  }

  /** Sets the event to signaled state. */
  public void set() {
    WPIUtilJNI.setEvent(m_handle);
  }

  /** Sets the event to non-signaled state. */
  public void reset() {
    WPIUtilJNI.resetEvent(m_handle);
  }

  private int m_handle;
}
