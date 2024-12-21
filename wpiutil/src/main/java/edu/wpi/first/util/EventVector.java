// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

/** A thread-safe container for handling events. */
public class EventVector {
  private final ReentrantLock m_lock = new ReentrantLock();
  private final List<Integer> m_events = new ArrayList<>();

  /** Default constructor. */
  public EventVector() {}

  /**
   * Adds an event to the event vector.
   *
   * @param handle The event to add
   */
  public void add(int handle) {
    m_lock.lock();
    try {
      m_events.add(handle);
    } finally {
      m_lock.unlock();
    }
  }

  /**
   * Removes an event from the event vector.
   *
   * @param handle The event to remove
   */
  public void remove(int handle) {
    m_lock.lock();
    try {
      m_events.removeIf(x -> x == handle);
    } finally {
      m_lock.unlock();
    }
  }

  /** Wakes up all events in the event vector. */
  public void wakeup() {
    m_lock.lock();
    try {
      for (int eventHandle : m_events) {
        WPIUtilJNI.setEvent(eventHandle);
      }
    } finally {
      m_lock.unlock();
    }
  }
}
