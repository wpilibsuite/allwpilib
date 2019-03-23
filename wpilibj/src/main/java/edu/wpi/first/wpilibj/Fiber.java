/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * A wrapper around a callable object with an execution period.
 *
 * <p>FiberScheduler can be used to run it periodically.
 */
public class Fiber implements Comparable<Fiber> {
  long m_period; // us
  long m_expirationTime; // us
  final Runnable m_callback;

  /**
   * Fiber constructor.
   *
   * @param period   The period between callback invocations in seconds with
   *                 microsecond resolution.
   * @param callback The function to call every period.
   */
  public Fiber(double period, Runnable callback) {
    m_period = (long) (period * 1.0e6);
    m_callback = callback;
  }

  @Override
  public int compareTo(Fiber rhs) {
    // Elements with sooner expiration times are sorted as lesser. The head of
    // Java's PriorityQueue is the least element.
    if (m_expirationTime < rhs.m_expirationTime) {
      return -1;
    } else if (m_expirationTime > rhs.m_expirationTime) {
      return 1;
    } else {
      return 0;
    }
  }
}
