/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.InterruptJNI;
import edu.wpi.first.wpilibj.util.AllocationException;
import edu.wpi.first.wpilibj.util.CheckedAllocationException;

/**
 * Base for sensors to be used with interrupts
 */
public abstract class InterruptableSensorBase extends SensorBase {

  public static enum WaitResult {
    kTimeout(0x0), kRisingEdge(0x1), kFallingEdge(0x100), kBoth(0x101);

    public final int value;

    public static WaitResult valueOf(int value) {
      for (WaitResult mode : values()) {
        if (mode.value == value) {
          return mode;
        }
      }
      return null;
    }


    private WaitResult(int value) {
      this.value = value;
    }
  }

  /**
   * The interrupt resource
   */
  protected long m_interrupt = 0;

  /**
   * Flags if the interrupt being allocated is synchronous
   */
  protected boolean m_isSynchronousInterrupt = false;

  /**
   * The index of the interrupt
   */
  protected int m_interruptIndex;
  /**
   * Resource manager
   */
  protected static Resource interrupts = new Resource(8);

  /**
   * Create a new InterrupatableSensorBase
   */
  public InterruptableSensorBase() {
    m_interrupt = 0;
  }

  /**
   * @return true if this is an analog trigger
   */
  abstract boolean getAnalogTriggerForRouting();

  /**
   * @return channel routing number
   */
  abstract int getChannelForRouting();

  /**
   * @return module routing number
   */
  abstract byte getModuleForRouting();

  /**
   * Request one of the 8 interrupts asynchronously on this digital input.
   *
   * @param handler The {@link InterruptHandlerFunction} that contains the
   *        method {@link InterruptHandlerFunction#interruptFired(int, Object)}
   *        that will be called whenever there is an interrupt on this device.
   *        Request interrupts in synchronous mode where the user program
   *        interrupt handler will be called when an interrupt occurs. The
   *        default is interrupt on rising edges only.
   */
  public void requestInterrupts(InterruptHandlerFunction<?> handler) {
    if (m_interrupt != 0) {
      throw new AllocationException("The interrupt has already been allocated");
    }

    allocateInterrupts(false);

    assert (m_interrupt != 0);

    InterruptJNI.requestInterrupts(m_interrupt, getModuleForRouting(), getChannelForRouting(),
        getAnalogTriggerForRouting());
    setUpSourceEdge(true, false);
    InterruptJNI.attachInterruptHandler(m_interrupt, handler.function,
        handler.overridableParameter());
  }

  /**
   * Request one of the 8 interrupts synchronously on this digital input.
   * Request interrupts in synchronous mode where the user program will have to
   * explicitly wait for the interrupt to occur using {@link #waitForInterrupt}.
   * The default is interrupt on rising edges only.
   */
  public void requestInterrupts() {
    if (m_interrupt != 0) {
      throw new AllocationException("The interrupt has already been allocated");
    }

    allocateInterrupts(true);

    assert (m_interrupt != 0);

    InterruptJNI.requestInterrupts(m_interrupt, getModuleForRouting(), getChannelForRouting(),
        getAnalogTriggerForRouting());
    setUpSourceEdge(true, false);

  }

  /**
   * Allocate the interrupt
   *
   * @param watcher true if the interrupt should be in synchronous mode where
   *        the user program will have to explicitly wait for the interrupt to
   *        occur.
   */
  protected void allocateInterrupts(boolean watcher) {
    try {
      m_interruptIndex = interrupts.allocate();
    } catch (CheckedAllocationException e) {
      throw new AllocationException("No interrupts are left to be allocated");
    }
    m_isSynchronousInterrupt = watcher;

    m_interrupt =
        InterruptJNI.initializeInterrupts(m_interruptIndex, watcher);
  }

  /**
   * Cancel interrupts on this device. This deallocates all the chipobject
   * structures and disables any interrupts.
   */
  public void cancelInterrupts() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    InterruptJNI.cleanInterrupts(m_interrupt);
    m_interrupt = 0;
    interrupts.free(m_interruptIndex);
  }

  /**
   * In synchronous mode, wait for the defined interrupt to occur.
   *
   * @param timeout Timeout in seconds
   * @param ignorePrevious If true, ignore interrupts that happened before
   *        waitForInterrupt was called.
   * @return Result of the wait.
   */
  public WaitResult waitForInterrupt(double timeout, boolean ignorePrevious) {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    int result = InterruptJNI.waitForInterrupt(m_interrupt, timeout, ignorePrevious);

    return WaitResult.valueOf(result);
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
   * Enable interrupts to occur on this input. Interrupts are disabled when the
   * RequestInterrupt call is made. This gives time to do the setup of the other
   * options before starting to field interrupts.
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
   * Return the timestamp for the rising interrupt that occurred most recently.
   * This is in the same time domain as getClock(). The rising-edge interrupt
   * should be enabled with {@link #setUpSourceEdge}
   *$
   * @return Timestamp in seconds since boot.
   */
  public double readRisingTimestamp() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    return InterruptJNI.readRisingTimestamp(m_interrupt);
  }

  /**
   * Return the timestamp for the falling interrupt that occurred most recently.
   * This is in the same time domain as getClock(). The falling-edge interrupt
   * should be enabled with {@link #setUpSourceEdge}
   *$
   * @return Timestamp in seconds since boot.
   */
  public double readFallingTimestamp() {
    if (m_interrupt == 0) {
      throw new IllegalStateException("The interrupt is not allocated.");
    }
    return InterruptJNI.readFallingTimestamp(m_interrupt);
  }

  /**
   * Set which edge to trigger interrupts on
   *
   * @param risingEdge true to interrupt on rising edge
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
