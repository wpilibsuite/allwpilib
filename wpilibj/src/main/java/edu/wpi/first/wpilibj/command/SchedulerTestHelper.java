/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.command;

import java.util.concurrent.TimeUnit;

/**
 * Integration test helper class for integration tests, like command group
 * tests, that need the Scheduler to run, so that all commands in the group get
 * executed in the right order at the right time. Use of this class requires
 * that your test class be decorated with
 * {@code @ExtendsWith(MockHardwareExtension.class)}.
 */
public class SchedulerTestHelper {
  private int m_heartbeatMs;

  /**
   * Initialized a runner with your own heartbeat (in ms).
   */
  public SchedulerTestHelper(int heartbeatMs) {
    this.m_heartbeatMs = heartbeatMs;
    Scheduler.getInstance().removeAll();
    Scheduler.getInstance().enable();
  }

  /**
   * Default constructor initialized runner heartbeat to 20 ms.
   */
  public SchedulerTestHelper() {
    this(20);
  }

  /**
   * Do not let an instance of this class go to the garbage collector without
   * calling the destroy method first.
   */
  public void destroy() {
    Scheduler.getInstance().disable();
    Scheduler.getInstance().close();
  }

  /**
   * Run our simulated robot command scheduler for a given amount of time.
   * 
   * @param durationInMs            Duration to run in milliseconds
   * @throws InterruptedException   Thrown if sleeping interrupted
   */
  public void runForDuration(long durationInMs) throws InterruptedException {
    long start = System.nanoTime();
    while (System.nanoTime() < (start + TimeUnit.MILLISECONDS.toNanos(durationInMs))) {
      Scheduler.getInstance().run();
      Thread.sleep(m_heartbeatMs);
    }
  }
}
