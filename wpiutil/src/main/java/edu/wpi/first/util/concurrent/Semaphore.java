// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.concurrent;

import edu.wpi.first.util.WPIUtilJNI;

/**
 * A semaphore for synchronization.
 *
 * <p>Semaphores keep an internal counter. Releasing the semaphore increases the count. A semaphore
 * with a non-zero count is considered signaled. When a waiter wakes up it atomically decrements the
 * count by 1. This is generally useful in a single-supplier, multiple-consumer scenario.
 */
public final class Semaphore implements AutoCloseable {
  /**
   * Constructor.
   *
   * @param initialCount initial value for the semaphore's internal counter
   * @param maximumCount maximum value for the samephore's internal counter
   */
  public Semaphore(int initialCount, int maximumCount) {
    m_handle = WPIUtilJNI.createSemaphore(initialCount, maximumCount);
  }

  /**
   * Constructor. Maximum count is Integer.MAX_VALUE.
   *
   * @param initialCount initial value for the semaphore's internal counter
   */
  public Semaphore(int initialCount) {
    this(initialCount, Integer.MAX_VALUE);
  }

  /** Constructor. Initial count is 0, maximum count is Integer.MAX_VALUE. */
  public Semaphore() {
    this(0, Integer.MAX_VALUE);
  }

  @Override
  public void close() {
    if (m_handle != 0) {
      WPIUtilJNI.destroySemaphore(m_handle);
      m_handle = 0;
    }
  }

  /**
   * Gets the semaphore handle (e.g. for waitForObject).
   *
   * @return handle
   */
  public int getHandle() {
    return m_handle;
  }

  /**
   * Releases N counts of the semaphore.
   *
   * @param releaseCount amount to add to semaphore's internal counter; must be positive
   * @return True on successful release, false on failure (e.g. release count would exceed maximum
   *     value, or handle invalid)
   */
  public boolean release(int releaseCount) {
    return WPIUtilJNI.releaseSemaphore(m_handle, releaseCount);
  }

  /**
   * Releases 1 count of the semaphore.
   *
   * @return True on successful release, false on failure (e.g. release count would exceed maximum
   *     value, or handle invalid)
   */
  public boolean release() {
    return release(1);
  }

  private int m_handle;
}
