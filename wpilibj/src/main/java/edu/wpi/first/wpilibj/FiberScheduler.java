/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.io.Closeable;
import java.util.PriorityQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

/**
 * A class for that schedules Fibers to run on an internal thread.
 */
public class FiberScheduler implements Closeable {
  private final Thread m_thread;
  private boolean m_running;

  private final PriorityQueue<Fiber> m_fibers = new PriorityQueue<>();
  private final ReentrantLock m_queueMutex = new ReentrantLock();
  private final Condition m_schedulerWaiter = m_queueMutex.newCondition();

  /**
   * FiberScheduler constructor.
   *
   * <p>Constructs a non-real-time thread on which to run scheduled fibers.
   */
  public FiberScheduler() {
    m_thread = new Thread(() -> schedulerFunc());
    m_running = true;
    m_thread.start();
  }

  /**
   * FiberScheduler constructor.
   *
   * <p>Constructs a real-time thread on which to run scheduled fibers.
   *
   * @param priority The priority at which to run the internal thread (1..99
   *                 where 1 is highest priority).
   */
  public FiberScheduler(int priority) {
    m_thread = new Thread(() -> {
      Threads.setCurrentThreadPriority(true, priority);
      schedulerFunc();
    });
    m_running = true;
    m_thread.start();
  }

  @Override
  public void close() {
    m_queueMutex.lock();
    try {
      m_fibers.clear();
      m_running = false;
      m_schedulerWaiter.signalAll();
    } finally {
      m_queueMutex.unlock();
    }

    try {
      m_thread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      ex.printStackTrace();
    }
  }

  /**
   * Add a fiber to the scheduler.
   *
   * <p>The fiber will automaticaly start running after its period has elapsed.
   *
   * @param fiber The fiber to add.
   */
  public void add(Fiber fiber) {
    long startTime = RobotController.getFPGATime();

    m_queueMutex.lock();
    try {
      m_fibers.remove(fiber);
      fiber.m_expirationTime = startTime + fiber.m_period;
      m_fibers.add(fiber);
      m_schedulerWaiter.signalAll();
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Remove a fiber from the scheduler.
   *
   * <p>This will wait until the current fiber has stopped running, then remove
   * the given fiber from the scheduler.
   *
   * @param fiber The fiber to remove.
   */
  public void remove(Fiber fiber) {
    m_queueMutex.lock();
    try {
      m_fibers.remove(this);
      m_schedulerWaiter.signalAll();
    } finally {
      m_queueMutex.unlock();
    }
  }

  private void schedulerFunc() {
    m_queueMutex.lock();

    try {
      while (m_running) {
        if (m_fibers.size() > 0) {
          boolean timedOut = !awaitUntil(m_schedulerWaiter, m_fibers.peek().m_expirationTime);
          if (timedOut) {
            if (m_fibers.size() == 0 || m_fibers.peek().m_expirationTime
                > RobotController.getFPGATime()) {
              continue;
            }

            // If the condition variable timed out, a Fiber is ready to run
            Fiber fiber = m_fibers.poll();

            m_queueMutex.unlock();
            fiber.m_callback.run();
            m_queueMutex.lock();

            // Reschedule Fiber
            fiber.m_expirationTime = RobotController.getFPGATime() + fiber.m_period;
            m_fibers.add(fiber);
          }
          // Otherwise, a Fiber removed itself from the queue (it notifies the
          // scheduler of this) or a spurious wakeup occurred, so just rewait
          // with the soonest watchdog timeout.
        } else {
          while (m_fibers.size() == 0) {
            m_schedulerWaiter.awaitUninterruptibly();
          }
        }
      }
    } finally {
      m_queueMutex.unlock();
    }
  }

  /**
   * Wrapper emulating functionality of C++'s std::condition_variable::wait_until().
   *
   * @param cond The condition variable on which to wait.
   * @param time The time at which to stop waiting.
   * @return False if the deadline has elapsed upon return, else true.
   */
  private static boolean awaitUntil(Condition cond, long time) {
    long delta = time - RobotController.getFPGATime();
    try {
      return cond.await(delta, TimeUnit.MICROSECONDS);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      ex.printStackTrace();
    }

    return true;
  }
}
