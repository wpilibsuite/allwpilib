// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.both;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTimeoutPreemptively;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.time.Duration;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

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

  @AfterEach
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

  @Test
  public void testSingleInterruptsTriggering() throws Exception {
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

      assertTimeoutPreemptively(
          Duration.ofSeconds(1),
          () -> {
            while (!hasFired.get()) {
              Timer.delay(0.005);
            }
          });

      assertEquals(1, counter.get(), "The interrupt did not fire the expected number of times");

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

  @Test
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
        assertTimeoutPreemptively(
            Duration.ofSeconds(2),
            () -> {
              // Wait for the interrupt to complete before moving on
              while (!hasFired.getAndSet(false)) {
                Timer.delay(0.005);
              }
            });
      }
      // Then
      assertEquals(
          fireCount, counter.get(), "The interrupt did not fire the expected number of times");
    }
  }

  /** The timeout length for this test in seconds. */
  private static final int kSynchronousTimeout = 5;

  @Timeout(kSynchronousTimeout)
  @Test
  public void testSynchronousInterruptsTriggering() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      final double synchronousDelay = kSynchronousTimeout / 2.0;
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
      interrupt.waitForInterrupt(kSynchronousTimeout * 2);
      final long stopTimeStamp = RobotController.getFPGATime();

      // Then
      // The test will not have timed out and:
      final double interruptRunTime = (stopTimeStamp - startTimeStamp) * 1e-6;
      assertEquals(
          synchronousDelay,
          interruptRunTime,
          0.1,
          "The interrupt did not run for the expected amount of time (units in seconds)");
    }
  }

  @Timeout(kSynchronousTimeout)
  @Test
  public void testSynchronousInterruptsWaitResultTimeout() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      SynchronousInterrupt.WaitResult result = interrupt.waitForInterrupt(kSynchronousTimeout / 2);

      assertEquals(
          result,
          SynchronousInterrupt.WaitResult.kTimeout,
          "The interrupt did not time out correctly.");
    }
  }

  @Timeout(kSynchronousTimeout)
  @Test
  public void testSynchronousInterruptsWaitResultRisingEdge() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      final double synchronousDelay = kSynchronousTimeout / 2.0;
      final Runnable runnable =
          () -> {
            Timer.delay(synchronousDelay);
            setInterruptLow();
            setInterruptHigh();
          };

      new Thread(runnable).start();
      // Delay for twice as long as the timeout so the test should fail first
      SynchronousInterrupt.WaitResult result = interrupt.waitForInterrupt(kSynchronousTimeout * 2);

      assertEquals(
          result,
          SynchronousInterrupt.WaitResult.kRisingEdge,
          "The interrupt did not fire on the rising edge.");
    }
  }

  @Timeout(kSynchronousTimeout)
  @Test
  public void testSynchronousInterruptsWaitResultFallingEdge() {
    try (SynchronousInterrupt interrupt = new SynchronousInterrupt(getSource())) {
      // Given
      interrupt.setInterruptEdges(false, true);

      final double synchronousDelay = kSynchronousTimeout / 2.0;
      final Runnable runnable =
          () -> {
            Timer.delay(synchronousDelay);
            setInterruptHigh();
            setInterruptLow();
          };

      new Thread(runnable).start();
      // Delay for twice as long as the timeout so the test should fail first
      SynchronousInterrupt.WaitResult result = interrupt.waitForInterrupt(kSynchronousTimeout * 2);

      assertEquals(
          result,
          SynchronousInterrupt.WaitResult.kFallingEdge,
          "The interrupt did not fire on the falling edge.");
    }
  }

  @Test
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
        assertTimeoutPreemptively(
            Duration.ofSeconds(4),
            () -> {
              // Wait for the interrupt to complete before moving on
              while (!interruptComplete.getAndSet(false)) {
                Timer.delay(0.005);
              }
            });
      }
      interrupt.disable();
      for (int i = 0; i < fireCount; i++) {
        setInterruptLow();
        setInterruptHigh();
        // Just wait because the interrupt should not fire
        Timer.delay(0.005);
      }

      assertEquals(
          fireCount, counter.get(), "The interrupt did not fire the expected number of times");
    }
  }
}
