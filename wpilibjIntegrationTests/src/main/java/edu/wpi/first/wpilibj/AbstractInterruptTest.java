// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.hamcrest.Matchers.both;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import org.junit.After;
import org.junit.Test;

/**
 * This class should not be run as a test explicitly. Instead it should be extended by tests that
 * use DigitalSource.
 */
public abstract class AbstractInterruptTest extends AbstractComsSetup {
  private DigitalSource m_source = null;

  private DigitalSource getSource() {
    if (m_source == null) {
      m_source = giveSource();
    }
    return m_source;
  }

  @After
  public void interruptTeardown() {
    if (m_source != null) {
      freeSource();
      m_source = null;
    }
  }

  /** Give the sensor source that interrupts can be attached to. */
  abstract DigitalSource giveSource();

  /** Cleans up the sensor source. This is only called if {@link #giveSource()} is called. */
  abstract void freeSource();

  /** Perform whatever action is required to set the interrupt high. */
  abstract void setInterruptHigh();

  /** Perform whatever action is required to set the interrupt low. */
  abstract void setInterruptLow();

  @Test(timeout = 1000)
  public void testSingleInterruptsTriggering() {
    // Given
    // final InterruptCounter counter = new InterruptCounter();
    // TestInterruptHandlerFunction function = new
    // TestInterruptHandlerFunction(counter);

    AtomicBoolean hasFired = new AtomicBoolean(false);
    AtomicInteger counter = new AtomicInteger(0);
    AtomicLong interruptFireTime = new AtomicLong();

    try (AsynchronousInterrupt interrupt =
        new AsynchronousInterrupt(
            getSource(),
            (a, b) -> {
              interruptFireTime.set(RobotController.getFPGATime());
              hasFired.set(true);
              counter.incrementAndGet();
            })) {
      interrupt.enable();
      setInterruptLow();
      Timer.delay(0.01);
      final long interruptTriggerTime = RobotController.getFPGATime();
      setInterruptHigh();
      while (!hasFired.get()) {
        Timer.delay(0.005);
      }

      assertEquals("The interrupt did not fire the expected number of times", 1, counter.get());

      final long range = 10000; // in microseconds
      assertThat(
          "The interrupt did not fire within the expected time period (values in milliseconds)",
          interruptFireTime.get(),
          both(greaterThan(interruptTriggerTime - range))
              .and(lessThan(interruptTriggerTime + range)));
      assertThat(
          "The readRisingTimestamp() did not return the correct value (values in seconds)",
          interrupt.getRisingTimestamp(),
          both(greaterThan((interruptTriggerTime - range) / 1e6))
              .and(lessThan((interruptTriggerTime + range) / 1e6)));
    }
  }

  @Test(timeout = 2000)
  public void testMultipleInterruptsTriggering() {
    AtomicBoolean hasFired = new AtomicBoolean(false);
    AtomicInteger counter = new AtomicInteger(0);

    try (AsynchronousInterrupt interrupt =
        new AsynchronousInterrupt(
            getSource(),
            (a, b) -> {
              hasFired.set(true);
              counter.incrementAndGet();
            })) {
      interrupt.enable();

      final int fireCount = 50;
      for (int i = 0; i < fireCount; i++) {
        setInterruptLow();
        setInterruptHigh();
        // Wait for the interrupt to complete before moving on
        while (!hasFired.getAndSet(false)) {
          Timer.delay(0.005);
        }
      }
      // Then
      assertEquals(
          "The interrupt did not fire the expected number of times", fireCount, counter.get());
    }
  }

  /** The timeout length for this test in seconds. */
  private static final int synchronousTimeout = 5;

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsTriggering() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      final double synchronousDelay = synchronousTimeout / 2.0;
      final Runnable runnable =
          () -> {
            Timer.delay(synchronousDelay);
            setInterruptLow();
            setInterruptHigh();
          };

      // When

      // Note: the long time value is used because doubles can flip if the robot
      // is left running for long enough
      final long startTimeStamp = RobotController.getFPGATime();
      new Thread(runnable).start();
      // Delay for twice as long as the timeout so the test should fail first
      interrupt.waitForInterrupt(synchronousTimeout * 2);
      final long stopTimeStamp = RobotController.getFPGATime();

      // Then
      // The test will not have timed out and:
      final double interruptRunTime = (stopTimeStamp - startTimeStamp) * 1e-6;
      assertEquals(
          "The interrupt did not run for the expected amount of time (units in seconds)",
          synchronousDelay,
          interruptRunTime,
          0.1);
    }
  }

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsWaitResultTimeout() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      SynchronousInterrupt.WaitResult result = interrupt.waitForInterrupt(synchronousTimeout / 2);

      assertEquals(
          "The interrupt did not time out correctly.",
          result,
          SynchronousInterrupt.WaitResult.kTimeout);
    }
  }

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsWaitResultRisingEdge() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      final double synchronousDelay = synchronousTimeout / 2.0;
      final Runnable runnable =
          () -> {
            Timer.delay(synchronousDelay);
            setInterruptLow();
            setInterruptHigh();
          };

      new Thread(runnable).start();
      // Delay for twice as long as the timeout so the test should fail first
      SynchronousInterrupt.WaitResult result = interrupt.waitForInterrupt(synchronousTimeout * 2);

      assertEquals(
          "The interrupt did not fire on the rising edge.",
          result,
          SynchronousInterrupt.WaitResult.kRisingEdge);
    }
  }

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsWaitResultFallingEdge() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      // Given
      interrupt.setInterruptEdges(false, true);

      final double synchronousDelay = synchronousTimeout / 2.0;
      final Runnable runnable =
          () -> {
            Timer.delay(synchronousDelay);
            setInterruptHigh();
            setInterruptLow();
          };

      new Thread(runnable).start();
      // Delay for twice as long as the timeout so the test should fail first
      SynchronousInterrupt.WaitResult result = interrupt.waitForInterrupt(synchronousTimeout * 2);

      assertEquals(
          "The interrupt did not fire on the falling edge.",
          result,
          SynchronousInterrupt.WaitResult.kFallingEdge);
    }
  }

  @Test(timeout = 4000)
  public void testDisableStopsInterruptFiring() {
    AtomicBoolean interruptComplete = new AtomicBoolean(false);
    AtomicInteger counter = new AtomicInteger(0);
    try (AsynchronousInterrupt interrupt =
        new AsynchronousInterrupt(
            getSource(),
            (a, b) -> {
              interruptComplete.set(true);
              counter.incrementAndGet();
            })) {
      interrupt.enable();
      final int fireCount = 50;
      for (int i = 0; i < fireCount; i++) {
        setInterruptLow();
        setInterruptHigh();
        // Wait for the interrupt to complete before moving on
        while (!interruptComplete.getAndSet(false)) {
          Timer.delay(0.005);
        }
      }
      interrupt.disable();
      for (int i = 0; i < fireCount; i++) {
        setInterruptLow();
        setInterruptHigh();
        // Just wait because the interrupt should not fire
        Timer.delay(0.005);
      }

      assertEquals(
          "The interrupt did not fire the expected number of times", fireCount, counter.get());
    }
  }
}
