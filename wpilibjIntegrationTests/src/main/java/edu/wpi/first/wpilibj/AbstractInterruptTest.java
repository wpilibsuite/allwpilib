/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.After;
import org.junit.Test;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.hamcrest.Matchers.both;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.lessThan;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertThat;

/**
 * This class should not be run as a test explicitly. Instead it should be extended by tests that
 * use the InterruptableSensorBase.
 */
public abstract class AbstractInterruptTest extends AbstractComsSetup {
  private InterruptableSensorBase m_interruptable = null;

  private InterruptableSensorBase getInterruptable() {
    if (m_interruptable == null) {
      m_interruptable = giveInterruptableSensorBase();
    }
    return m_interruptable;
  }


  @After
  public void interruptTeardown() {
    if (m_interruptable != null) {
      freeInterruptableSensorBase();
      m_interruptable = null;
    }
  }

  /**
   * Give the interruptable sensor base that interrupts can be attached to.
   */
  abstract InterruptableSensorBase giveInterruptableSensorBase();

  /**
   * Cleans up the interruptable sensor base. This is only called if {@link
   * #giveInterruptableSensorBase()} is called.
   */
  abstract void freeInterruptableSensorBase();

  /**
   * Perform whatever action is required to set the interrupt high.
   */
  abstract void setInterruptHigh();

  /**
   * Perform whatever action is required to set the interrupt low.
   */
  abstract void setInterruptLow();


  private class InterruptCounter {
    private final AtomicInteger m_count = new AtomicInteger();

    void increment() {
      m_count.addAndGet(1);
    }

    int getCount() {
      return m_count.get();
    }
  }

  private class TestInterruptHandlerFunction extends InterruptHandlerFunction<InterruptCounter> {
    protected final AtomicBoolean m_exceptionThrown = new AtomicBoolean(false);
    /**
     * Stores the time that the interrupt fires.
     */
    final AtomicLong m_interruptFireTime = new AtomicLong();
    /**
     * Stores if the interrupt has completed at least once.
     */
    final AtomicBoolean m_interruptComplete = new AtomicBoolean(false);
    protected Exception m_ex;
    final InterruptCounter m_counter;

    TestInterruptHandlerFunction(InterruptCounter counter) {
      m_counter = counter;
    }

    @Override
    public void interruptFired(int interruptAssertedMask, InterruptCounter param) {
      m_interruptFireTime.set(Utility.getFPGATime());
      m_counter.increment();
      try {
        // This won't cause the test to fail
        assertSame(m_counter, param);
      } catch (Exception ex) {
        // So we must throw the exception within the test
        m_exceptionThrown.set(true);
        m_ex = ex;
      }
      m_interruptComplete.set(true);
    }

    @Override
    public InterruptCounter overridableParameter() {
      return m_counter;
    }
  }

  @Test(timeout = 1000)
  public void testSingleInterruptsTriggering() throws Exception {
    // Given
    final InterruptCounter counter = new InterruptCounter();
    TestInterruptHandlerFunction function = new TestInterruptHandlerFunction(counter);

    // When
    getInterruptable().requestInterrupts(function);
    getInterruptable().enableInterrupts();

    setInterruptLow();
    Timer.delay(0.01);
    // Note: Utility.getFPGATime() is used because double values can turn over
    // after the robot has been running for a long time
    final long interruptTriggerTime = Utility.getFPGATime();
    setInterruptHigh();

    // Delay until the interrupt is complete
    while (!function.m_interruptComplete.get()) {
      Timer.delay(.005);
    }


    // Then
    assertEquals("The interrupt did not fire the expected number of times", 1, counter.getCount());
    // If the test within the interrupt failed
    if (function.m_exceptionThrown.get()) {
      throw function.m_ex;
    }
    final long range = 10000; // in microseconds
    assertThat(
        "The interrupt did not fire within the expected time period (values in milliseconds)",
        function.m_interruptFireTime.get(),
        both(greaterThan(interruptTriggerTime - range)).and(lessThan(interruptTriggerTime
            + range)));
    assertThat(
        "The readRisingTimestamp() did not return the correct value (values in seconds)",
        getInterruptable().readRisingTimestamp(),
        both(greaterThan((interruptTriggerTime - range) / 1e6)).and(
            lessThan((interruptTriggerTime + range) / 1e6)));
  }

  @Test(timeout = 2000)
  public void testMultipleInterruptsTriggering() throws Exception {
    // Given
    final InterruptCounter counter = new InterruptCounter();
    TestInterruptHandlerFunction function = new TestInterruptHandlerFunction(counter);

    // When
    getInterruptable().requestInterrupts(function);
    getInterruptable().enableInterrupts();

    final int fireCount = 50;
    for (int i = 0; i < fireCount; i++) {
      setInterruptLow();
      setInterruptHigh();
      // Wait for the interrupt to complete before moving on
      while (!function.m_interruptComplete.getAndSet(false)) {
        Timer.delay(.005);
      }
    }
    // Then
    assertEquals("The interrupt did not fire the expected number of times", fireCount,
        counter.getCount());
  }

  /**
   * The timeout length for this test in seconds.
   */
  private static final int synchronousTimeout = 5;

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsTriggering() {
    // Given
    getInterruptable().requestInterrupts();

    final double synchronousDelay = synchronousTimeout / 2.;
    final Runnable runnable = () -> {
      Timer.delay(synchronousDelay);
      setInterruptLow();
      setInterruptHigh();
    };

    // When

    // Note: the long time value is used because doubles can flip if the robot
    // is left running for long enough
    final long startTimeStamp = Utility.getFPGATime();
    new Thread(runnable).start();
    // Delay for twice as long as the timeout so the test should fail first
    getInterruptable().waitForInterrupt(synchronousTimeout * 2);
    final long stopTimeStamp = Utility.getFPGATime();

    // Then
    // The test will not have timed out and:
    final double interruptRunTime = (stopTimeStamp - startTimeStamp) * 1e-6;
    assertEquals("The interrupt did not run for the expected amount of time (units in seconds)",
        synchronousDelay, interruptRunTime, .1);
  }

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsWaitResultTimeout() {
    // Given
    getInterruptable().requestInterrupts();

    //Don't fire interrupt. Expect it to timeout.
    InterruptableSensorBase.WaitResult result = getInterruptable()
        .waitForInterrupt(synchronousTimeout / 2);

    assertEquals("The interrupt did not time out correctly.", result, InterruptableSensorBase
        .WaitResult.kTimeout);
  }

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsWaitResultRisingEdge() {
    // Given
    getInterruptable().requestInterrupts();

    final double synchronousDelay = synchronousTimeout / 2.;
    final Runnable runnable = () -> {
      Timer.delay(synchronousDelay);
      setInterruptLow();
      setInterruptHigh();
    };

    new Thread(runnable).start();
    // Delay for twice as long as the timeout so the test should fail first
    InterruptableSensorBase.WaitResult result = getInterruptable()
        .waitForInterrupt(synchronousTimeout * 2);

    assertEquals("The interrupt did not fire on the rising edge.", result,
        InterruptableSensorBase.WaitResult.kRisingEdge);
  }

  @Test(timeout = (long) (synchronousTimeout * 1e3))
  public void testSynchronousInterruptsWaitResultFallingEdge() {
    // Given
    getInterruptable().requestInterrupts();
    getInterruptable().setUpSourceEdge(false, true);

    final double synchronousDelay = synchronousTimeout / 2.;
    final Runnable runnable = () -> {
      Timer.delay(synchronousDelay);
      setInterruptHigh();
      setInterruptLow();
    };

    new Thread(runnable).start();
    // Delay for twice as long as the timeout so the test should fail first
    InterruptableSensorBase.WaitResult result = getInterruptable()
        .waitForInterrupt(synchronousTimeout * 2);

    assertEquals("The interrupt did not fire on the falling edge.", result,
        InterruptableSensorBase.WaitResult.kFallingEdge);
  }


  @Test(timeout = 4000)
  public void testDisableStopsInterruptFiring() {
    final InterruptCounter counter = new InterruptCounter();
    TestInterruptHandlerFunction function = new TestInterruptHandlerFunction(counter);

    // When
    getInterruptable().requestInterrupts(function);
    getInterruptable().enableInterrupts();

    final int fireCount = 50;
    for (int i = 0; i < fireCount; i++) {
      setInterruptLow();
      setInterruptHigh();
      // Wait for the interrupt to complete before moving on
      while (!function.m_interruptComplete.getAndSet(false)) {
        Timer.delay(.005);
      }
    }
    getInterruptable().disableInterrupts();
    // TestBench.out().println("Finished disabling the robot");

    for (int i = 0; i < fireCount; i++) {
      setInterruptLow();
      setInterruptHigh();
      // Just wait because the interrupt should not fire
      Timer.delay(.005);
    }

    // Then
    assertEquals("The interrupt did not fire the expected number of times", fireCount,
        counter.getCount());
  }

}
