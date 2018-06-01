/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Takes an arbitrary list of PIDOutputBuffers and outputs the sum of their outputs to a PIDOutput.
 */
public class PIDOutputSummer {
  private Buffer[] m_buffers;

  private PIDOutput m_output;
  private Notifier m_notifier = new Notifier(this::setOutput);

  private class Buffer {
    public PIDOutputBuffer m_buffer;
    public boolean m_positive;

    Buffer(PIDOutputBuffer buffer, boolean positive) {
      m_buffer = buffer;
      m_positive = positive;
    }
  }

  /**
   * Appends buffer and associated sign to array.
   *
   * @param output   The output for the sum.
   * @param buffer   The PIDOutputBuffer object to add to the array for round
   *                 robin.
   * @param positive If true, adds buffer output; otherwise, subtracts buffer
   *                 output.
   * @param buffers  The other PIDOutputBuffer objects and bools.
   */
  public PIDOutputSummer(PIDOutput output, PIDOutputBuffer buffer, boolean positive,
      Object... buffers) {
    m_output = output;

    m_buffers = new Buffer[1 + buffers.length / 2];

    m_buffers[0] = new Buffer(buffer, positive);
    for (int i = 0; i < buffers.length; i += 2) {
      m_buffers[1 + i / 2] = new Buffer((PIDOutputBuffer) buffers[i], (boolean) buffers[i + 1]);
    }
  }

  /**
   * Starts periodic output.
   *
   * @param period The loop time for doing calculations in seconds. The default
   *               period is 50ms.
   */
  public void enable(double period) {
    m_notifier.startPeriodic(period);
  }

  /**
   * Starts periodic output.
   */
  public void enable() {
    m_notifier.startPeriodic(0.05);
  }

  /**
   * Stops periodic output.
   */
  public void disable() {
    m_notifier.stop();
  }

  private void setOutput() {
    double sum = 0.0;

    for (Buffer buffer : m_buffers) {
      if (buffer.m_positive) {
        sum += buffer.m_buffer.getOutput();
      } else {
        sum -= buffer.m_buffer.getOutput();
      }
    }

    m_output.pidWrite(sum);
  }
}
