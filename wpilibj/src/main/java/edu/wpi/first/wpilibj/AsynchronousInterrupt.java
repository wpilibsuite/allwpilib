/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.BiConsumer;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

public class AsynchronousInterrupt implements AutoCloseable {
  private final BiConsumer<Boolean, Boolean> m_callback;
  private final SynchronousInterrupt m_interrupt;

  private final AtomicBoolean m_keepRunning = new AtomicBoolean(false);
  private Thread m_thread;

  public AsynchronousInterrupt(DigitalSource source, BiConsumer<Boolean, Boolean> callback) {
    m_callback = requireNonNullParam(callback, "callback", "AsynchronousInterrupt");
    m_interrupt = new SynchronousInterrupt(source);
  }

  @Override
  public void close() throws Exception {
    disable();
    m_interrupt.close();
  }

  public void enable() {
    if (m_keepRunning.get()) {
      return;
    }

    m_keepRunning.set(true);
    m_thread = new Thread(this::threadMain);
  }

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

  public void setInterruptEdges(boolean risingEdge, boolean fallingEdge) {
    m_interrupt.setInterruptEdges(risingEdge, fallingEdge);
  }

  public double getRisingTimestamp() {
    return m_interrupt.getRisingTimestamp();
  }

  public double getFallingTimestamp() {
    return m_interrupt.getFallingTimestamp();
  }

  private void threadMain() {
    while (m_keepRunning.get()) {
      var result = m_interrupt.waitForInterruptRaw(10, false);
      if (!m_keepRunning.get()) {
        break;
      }
      if (result == 0) {
        continue;
      }
      m_callback.accept((result & 0x1) != 0, (result & 0x100) != 0);
    }
  }

}
