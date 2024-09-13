// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.BiConsumer;

/**
 * Class for handling asynchronous interrupts using a callback thread.
 *
 * <p>By default, interrupts will occur on rising edge. Callbacks are disabled by default, and
 * enable() must be called before they will occur.
 *
 * <p>Both rising and falling edges can be indicated in one callback if both a rising and falling
 * edge occurred since the previous callback.
 *
 * <p>Synchronous (blocking) interrupts are handled by the SynchronousInterrupt class.
 */
public class AsynchronousInterrupt implements AutoCloseable {
  private final BiConsumer<Boolean, Boolean> m_callback;
  private final SynchronousInterrupt m_interrupt;

  private final AtomicBoolean m_keepRunning = new AtomicBoolean(false);
  private Thread m_thread;

  /**
   * Construct a new asynchronous interrupt using a Digital Source.
   *
   * <p>At construction, the interrupt will trigger on the rising edge.
   *
   * <p>Callbacks will not be triggered until enable() is called.
   *
   * <p>The first bool in the callback indicates the rising edge triggered the interrupt, the second
   * bool is falling edge.
   *
   * @param source The digital source to use.
   * @param callback The callback to call on an interrupt
   */
  public AsynchronousInterrupt(DigitalSource source, BiConsumer<Boolean, Boolean> callback) {
    m_callback = requireNonNullParam(callback, "callback", "AsynchronousInterrupt");
    m_interrupt = new SynchronousInterrupt(source);
  }

  /**
   * Closes the interrupt.
   *
   * <p>This does not close the associated digital source.
   *
   * <p>This will disable the interrupt if it is enabled.
   */
  @Override
  public void close() {
    disable();
    m_interrupt.close();
  }

  /**
   * Enables interrupt callbacks. Before this, callbacks will not occur. Does nothing if already
   * enabled.
   */
  public void enable() {
    if (m_keepRunning.get()) {
      return;
    }

    m_keepRunning.set(true);
    m_thread = new Thread(this::threadMain);
    m_thread.start();
  }

  /** Disables interrupt callbacks. Does nothing if already disabled. */
  public void disable() {
    m_keepRunning.set(false);
    m_interrupt.wakeupWaitingInterrupt();
    if (m_thread != null) {
      if (m_thread.isAlive()) {
        try {
          m_thread.interrupt();
          m_thread.join();
        } catch (InterruptedException ex) {
          Thread.currentThread().interrupt();
        }
      }
      m_thread = null;
    }
  }

  /**
   * Set which edges to trigger the interrupt on.
   *
   * @param risingEdge Trigger on rising edge
   * @param fallingEdge Trigger on falling edge
   */
  public void setInterruptEdges(boolean risingEdge, boolean fallingEdge) {
    m_interrupt.setInterruptEdges(risingEdge, fallingEdge);
  }

  /**
   * Get the timestamp of the last rising edge.
   *
   * <p>This function does not require the interrupt to be enabled to work.
   *
   * <p>This only works if rising edge was configured using setInterruptEdges.
   *
   * @return the timestamp in seconds relative to getFPGATime
   */
  public double getRisingTimestamp() {
    return m_interrupt.getRisingTimestamp();
  }

  /**
   * Get the timestamp of the last falling edge.
   *
   * <p>This function does not require the interrupt to be enabled to work.
   *
   * <p>This only works if falling edge was configured using setInterruptEdges.
   *
   * @return the timestamp in seconds relative to getFPGATime
   */
  public double getFallingTimestamp() {
    return m_interrupt.getFallingTimestamp();
  }

  private void threadMain() {
    while (m_keepRunning.get()) {
      var result = m_interrupt.waitForInterrupt(10, false);
      if (!m_keepRunning.get()) {
        break;
      }
      boolean rising = false;
      boolean falling = false;
      switch (result) {
        case kBoth -> {
          rising = true;
          falling = true;
        }
        case kFallingEdge -> {
          falling = true;
        }
        case kRisingEdge -> {
          rising = true;
        }
        default -> {
          continue;
        }
      }

      m_callback.accept(rising, falling);
    }
  }
}
