// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.InterruptJNI;

/**
 * Class for handling synchronous (blocking) interrupts.
 *
 * <p>By default, interrupts will occur on rising edge.
 *
 * <p>Asynchronous interrupts are handled by the AsynchronousInterrupt class.
 */
public class SynchronousInterrupt implements AutoCloseable {
  @SuppressWarnings("PMD.SingularField")
  private final DigitalSource m_source;

  private final int m_handle;

  /** Event trigger combinations for a synchronous interrupt. */
  public enum WaitResult {
    /** Timeout event. */
    kTimeout(0x0),
    /** Rising edge event. */
    kRisingEdge(0x1),
    /** Falling edge event. */
    kFallingEdge(0x100),
    /** Both rising and falling edge events. */
    kBoth(0x101);

    /** WaitResult value. */
    public final int value;

    WaitResult(int value) {
      this.value = value;
    }

    /**
     * Create a wait result enum.
     *
     * @param rising True if a rising edge occurred.
     * @param falling True if a falling edge occurred.
     * @return A wait result enum.
     */
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

  /**
   * Constructs a new synchronous interrupt using a DigitalSource.
   *
   * <p>At construction, the interrupt will trigger on the rising edge.
   *
   * @param source The digital source to use.
   */
  public SynchronousInterrupt(DigitalSource source) {
    m_source = requireNonNullParam(source, "source", "SynchronousInterrupt");
    m_handle = InterruptJNI.initializeInterrupts();
    InterruptJNI.requestInterrupts(
        m_handle, m_source.getPortHandleForRouting(), m_source.getAnalogTriggerTypeForRouting());
    InterruptJNI.setInterruptUpSourceEdge(m_handle, true, false);
  }

  /**
   * Closes the interrupt.
   *
   * <p>This does not close the associated digital source.
   */
  @Override
  public void close() {
    InterruptJNI.cleanInterrupts(m_handle);
  }

  /**
   * Wait for an interrupt.
   *
   * @param timeoutSeconds The timeout in seconds. 0 or less will return immediately.
   * @param ignorePrevious True to ignore if a previous interrupt has occurred, and only wait for a
   *     new trigger. False will consider if an interrupt has occurred since the last time the
   *     interrupt was read.
   * @return Result of which edges were triggered, or if a timeout occurred.
   */
  public WaitResult waitForInterrupt(double timeoutSeconds, boolean ignorePrevious) {
    long result = InterruptJNI.waitForInterrupt(m_handle, timeoutSeconds, ignorePrevious);

    // Rising edge result is the interrupt bit set in the byte 0xFF
    // Falling edge result is the interrupt bit set in the byte 0xFF00
    // Set any bit set to be true for that edge, and then conduct a logical AND on the 2 results
    // together to match the existing enum for all interrupts
    boolean rising = (result & 0xFF) != 0;
    boolean falling = (result & 0xFF00) != 0;
    return WaitResult.getValue(rising, falling);
  }

  /**
   * Wait for an interrupt, ignoring any previously occurring interrupts.
   *
   * @param timeoutSeconds The timeout in seconds. 0 or less will return immediately.
   * @return Result of which edges were triggered, or if a timeout occurred.
   */
  public WaitResult waitForInterrupt(double timeoutSeconds) {
    return waitForInterrupt(timeoutSeconds, true);
  }

  /**
   * Set which edges to trigger the interrupt on.
   *
   * @param risingEdge Trigger on rising edge
   * @param fallingEdge Trigger on falling edge
   */
  public void setInterruptEdges(boolean risingEdge, boolean fallingEdge) {
    InterruptJNI.setInterruptUpSourceEdge(m_handle, risingEdge, fallingEdge);
  }

  /**
   * Get the timestamp of the last rising edge.
   *
   * <p>This only works if rising edge was configured using setInterruptEdges.
   *
   * @return the timestamp in seconds relative to getFPGATime
   */
  public double getRisingTimestamp() {
    return InterruptJNI.readInterruptRisingTimestamp(m_handle) * 1e-6;
  }

  /**
   * Get the timestamp of the last falling edge.
   *
   * <p>This only works if falling edge was configured using setInterruptEdges.
   *
   * @return the timestamp in seconds relative to getFPGATime
   */
  public double getFallingTimestamp() {
    return InterruptJNI.readInterruptFallingTimestamp(m_handle) * 1e-6;
  }

  /** Force triggering of any waiting interrupt, which will be seen as a timeout. */
  public void wakeupWaitingInterrupt() {
    InterruptJNI.releaseWaitingInterrupt(m_handle);
  }
}
