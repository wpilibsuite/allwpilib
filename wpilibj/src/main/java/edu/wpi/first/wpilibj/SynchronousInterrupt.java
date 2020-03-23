/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.Objects;

import edu.wpi.first.hal.InterruptJNI;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

public class SynchronousInterrupt implements AutoCloseable {
  private final DigitalSource m_source;
  private final int m_handle;

  @SuppressWarnings("JavadocMethod")
  public enum WaitResult {
    kTimeout(0x0), kRisingEdge(0x1), kFallingEdge(0x100), kBoth(0x101);

    @SuppressWarnings("MemberName")
    public final int value;

    WaitResult(int value) {
      this.value = value;
    }

    public static WaitResult getValue(boolean rising, boolean falling) {
      if (rising && falling) {
        return kBoth;
      } else if (rising) {
        return kRisingEdge;
      } else if (falling) {
        return kFallingEdge;
      } else {
        return kTimeout;
      }
    }
  }

  public SynchronousInterrupt(DigitalSource source) {
    m_source = requireNonNullParam(source, "source", "SynchronousInterrupt");
    m_handle = InterruptJNI.initializeInterrupts(true);
    InterruptJNI.requestInterrupts(m_handle, m_source.getPortHandleForRouting(),
      m_source.getAnalogTriggerTypeForRouting());
  }


  @Override
  public void close() throws Exception {
    InterruptJNI.cleanInterrupts(m_handle);
  }

  int waitForInterruptRaw(double timeoutSeconds, boolean ignorePrevious) {
    return InterruptJNI.waitForInterrupt(m_handle, timeoutSeconds, ignorePrevious);
  }

  public WaitResult waitForInterrupt(double timeoutSeconds, boolean ignorePrevious) {
    int result = InterruptJNI.waitForInterrupt(m_handle, timeoutSeconds, ignorePrevious);

    // Rising edge result is the interrupt bit set in the byte 0xFF
    // Falling edge result is the interrupt bit set in the byte 0xFF00
    // Set any bit set to be true for that edge, and AND the 2 results
    // together to match the existing enum for all interrupts
    boolean rising = (result & 0xFF) != 0;
    boolean falling = (result & 0xFF00) != 0;
    return WaitResult.getValue(rising, falling);
  }

  public void setInterruptEdges(boolean risingEdge, boolean fallingEdge) {
    InterruptJNI.setInterruptUpSourceEdge(m_handle, risingEdge, fallingEdge);
  }

  public double getRisingTimestamp() {
    return InterruptJNI.readInterruptRisingTimestamp(m_handle) * 1e-6;
  }

  public double getFallingTimestamp() {
    return InterruptJNI.readInterruptFallingTimestamp(m_handle) * 1e-6;
  }

  public void wakeupWaitingInterrupt() {
    InterruptJNI.releaseWaitingInterrupt(m_handle);
  }
}
