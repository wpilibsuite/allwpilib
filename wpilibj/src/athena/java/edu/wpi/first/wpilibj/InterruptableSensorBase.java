/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.InterruptJNI;
import edu.wpi.first.wpilibj.util.AllocationException;


/**
 * Base for sensors to be used with interrupts.
 */
public abstract class InterruptableSensorBase extends SensorBase {

  @SuppressWarnings("JavadocMethod")
  public enum WaitResult {
    kTimeout(0x0), kRisingEdge(0x1), kFallingEdge(0x100), kBoth(0x101);

    @SuppressWarnings("MemberName")
    public final int value;

    private WaitResult(int value) {
      this.value = value;
    }
  }

  /**
   * The interrupt resource.
   */
  protected int m_interrupt = InterruptJNI.HalInvalidHandle;

  /**
   * Flags if the interrupt being allocated is synchronous.
   */
  protected boolean m_isSynchronousInterrupt = false;

  /**
   * Create a new InterrupatableSensorBase.
   */
  public InterruptableSensorBase() {
    m_interrupt = 0;
  }

  /**
   * If this is an analog trigger.
   *
   * @return true if this is an analog trigger.
   */
  public abstract int getAnalogTriggerTypeForRouting();

  /**
   * The channel routing number.
   *
   * @return channel routing number
   */
  public abstract int getPortHandleForRouting();

  /**
   * Request one of the 8 interrupts asynchronously on this digital input.
   *
   * @param handler The {@link InterruptHandlerFunction} that contains the method {@link
   *                InterruptHandlerFunction#interruptFired(int, Object)} that will be called
   *                whenever there is an interrupt on this device. Request interrupts in synchronous
   *                mode where the user program interrupt handler will be called when an interrupt
   *                occurs. The default is interrupt on rising edges only.
   */
  public void requestInterrupts(InterruptHandlerFunction<?> handler) {
    if (m_interrupt != 0) {
      throw new AllocationException("The interrupt has already been allocated");
    }

    allocateInterrupts(false);

    assert m_interrupt != 0;

    InterruptJNI.requestInterrupts(m_interrupt, getPortHandleForRouting(),
                                   getAnalogTriggerTypeForRouting());
    setUpSourceEdge(true, false);
    InterruptJNI.attachInterruptHandler(m_interrupt, handler.m_function,
        handler.overridableParameter());
  }

  /**
   * Request one of the 8 interrupts synchronously on this digital input. Request interrupts in
   * synchronous mode where the user program will have to explicitly wait for the interrupt to occur
   * using {@link #waitForInterrupt}. The default is interrupt on rising edges only.
   */
  public void requestInterrupts() {
    if (m_interrupt != 0) {
      throw new AllocationException("The interrupt has already been allocated");
    }

    allocateInterrupts(true);

    assert m_interrupt != 0;

    InterruptJNI.requestInterrupts(m_interrupt, getPortHandleForRouting(),
                                   getAnalogTriggerTypeForRouting());
    setUpSourceEdge(true, false);

  }

  /**
   * Allocate the interrupt
   *
   * @param watcher true if the interrupt should be in synchronous mode where the user program will
   *                have to explicitly wait for the interrupt to occur.
   */
  protected void allocateInterrupts(boolean watcher) {
    m_isSynchronousInterrupt = watcher;

    m_interrupt = InterruptJNI.initializeInterrupts(watcher);
  }

  /**
   * Cancel interrupts on this device. This deallocates all the chipobject structures and disables
   * any interrupts.
   */
  public void cancelInterrupts() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    InterruptJNI.cleanInterrupts(m_interrupt);
    m_interrupt = 0;
  }

  /**
   * In synchronous mode, wait for the defined interrupt to occur.
   *
   * @param timeout        Timeout in seconds
   * @param ignorePrevious If true, ignore interrupts that happened before waitForInterrupt was
   *                       called.
   * @return Result of the wait.
   */
  public WaitResult waitForInterrupt(double timeout, boolean ignorePrevious) {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    int result = InterruptJNI.waitForInterrupt(m_interrupt, timeout, ignorePrevious);

    // Rising edge result is the interrupt bit set in the byte 0xFF
    // Falling edge result is the interrupt bit set in the byte 0xFF00
    // Set any bit set to be true for that edge, and AND the 2 results
    // together to match the existing enum for all interrupts
    int rising = ((result & 0xFF) != 0) ? 0x1 : 0x0;
    int falling = ((result & 0xFF00) != 0) ? 0x0100 : 0x0;
    result = rising | falling;

    for (WaitResult mode : WaitResult.values()) {
      if (mode.value == result) {
        return mode;
      }
    }
    return null;
  }

  /**
   * In synchronous mode, wait for the defined interrupt to occur.
   *
   * @param timeout Timeout in seconds
   * @return Result of the wait.
   */
  public WaitResult waitForInterrupt(double timeout) {
    return waitForInterrupt(timeout, true);
  }

  /**
   * Enable interrupts to occur on this input. Interrupts are disabled when the RequestInterrupt
   * call is made. This gives time to do the setup of the other options before starting to field
   * interrupts.
   */
  public void enableInterrupts() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    if (m_isSynchronousInterrupt) {
      throw new IllegalStateException("You do not need to enable synchronous interrupts");
    }
    InterruptJNI.enableInterrupts(m_interrupt);
  }

  /**
   * Disable Interrupts without without deallocating structures.
   */
  public void disableInterrupts() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    if (m_isSynchronousInterrupt) {
      throw new IllegalStateException("You can not disable synchronous interrupts");
    }
    InterruptJNI.disableInterrupts(m_interrupt);
  }

  /**
   * Return the timestamp for the rising interrupt that occurred most recently. This is in the same
   * time domain as getClock(). The rising-edge interrupt should be enabled with {@link
   * #setUpSourceEdge}.
   *
   * @return Timestamp in seconds since boot.
   */
  public double readRisingTimestamp() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    return InterruptJNI.readInterruptRisingTimestamp(m_interrupt);
  }

  /**
   * Return the timestamp for the falling interrupt that occurred most recently. This is in the same
   * time domain as getClock(). The falling-edge interrupt should be enabled with {@link
   * #setUpSourceEdge}.
   *
   * @return Timestamp in seconds since boot.
   */
  public double readFallingTimestamp() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    return InterruptJNI.readInterruptFallingTimestamp(m_interrupt);
  }

  /**
   * Set which edge to trigger interrupts on.
   *
   * @param risingEdge  true to interrupt on rising edge
   * @param fallingEdge true to interrupt on falling edge
   */
  public void setUpSourceEdge(boolean risingEdge, boolean fallingEdge) {
    if (m_interrupt != 0) {
      InterruptJNI.setInterruptUpSourceEdge(m_interrupt, risingEdge,
          fallingEdge);
    } else {
      throw new IllegalArgumentException("You must call RequestInterrupts before setUpSourceEdge");
    }
  }
}
