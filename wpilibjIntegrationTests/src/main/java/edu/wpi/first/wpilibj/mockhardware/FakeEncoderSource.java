// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.mockhardware;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Timer;

/** Emulates a quadrature encoder. */
public class FakeEncoderSource implements AutoCloseable {
  private Thread m_task;
  private int m_count;
  private int m_milliSec;
  private boolean m_forward;
  private final DigitalOutput m_outputA;
  private final DigitalOutput m_outputB;
  private final boolean m_allocatedOutputs;

  /** Thread object that allows emulation of a quadrature encoder. */
  private static class QuadEncoderThread extends Thread {
    FakeEncoderSource m_encoder;

    QuadEncoderThread(FakeEncoderSource encode) {
      m_encoder = encode;
    }

    @Override
    public void run() {
      final DigitalOutput lead;
      final DigitalOutput lag;

      m_encoder.m_outputA.set(false);
      m_encoder.m_outputB.set(false);

      if (m_encoder.isForward()) {
        lead = m_encoder.m_outputA;
        lag = m_encoder.m_outputB;
      } else {
        lead = m_encoder.m_outputB;
        lag = m_encoder.m_outputA;
      }

      try {
        for (int i = 0; i < m_encoder.m_count; i++) {
          lead.set(true);
          Thread.sleep(m_encoder.m_milliSec);
          lag.set(true);
          Thread.sleep(m_encoder.m_milliSec);
          lead.set(false);
          Thread.sleep(m_encoder.m_milliSec);
          lag.set(false);
          Thread.sleep(m_encoder.m_milliSec);
        }
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    }
  }

  /**
   * Creates an encoder source using two ports.
   *
   * @param portA The A port
   * @param portB The B port
   */
  public FakeEncoderSource(int portA, int portB) {
    m_outputA = new DigitalOutput(portA);
    m_outputB = new DigitalOutput(portB);
    m_allocatedOutputs = true;
    initQuadEncoder();
  }

  /**
   * Creates the fake encoder using two digital outputs.
   *
   * @param outputA The A digital output
   * @param outputB The B digital output
   */
  public FakeEncoderSource(DigitalOutput outputA, DigitalOutput outputB) {
    m_outputA = outputA;
    m_outputB = outputB;
    m_allocatedOutputs = false;
    initQuadEncoder();
  }

  /** Frees the resource. */
  @Override
  public void close() {
    m_task = null;
    if (m_allocatedOutputs) {
      m_outputA.close();
      m_outputB.close();
    }
  }

  /** Common initialization code. */
  private void initQuadEncoder() {
    m_milliSec = 1;
    m_forward = true;
    m_task = new QuadEncoderThread(this);
    m_outputA.set(false);
    m_outputB.set(false);
  }

  /** Starts the thread. */
  public void start() {
    m_task.start();
  }

  /** Waits for thread to end. */
  public void complete() {
    try {
      m_task.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    m_task = new QuadEncoderThread(this);
    Timer.delay(0.01);
  }

  /** Runs and waits for thread to end before returning. */
  public void execute() {
    start();
    complete();
  }

  /**
   * Rate of pulses to send.
   *
   * @param milliSec Pulse Rate
   */
  public void setRate(int milliSec) {
    m_milliSec = milliSec;
  }

  /**
   * Set the number of pulses to simulate.
   *
   * @param count Pulse count
   */
  public void setCount(int count) {
    m_count = Math.abs(count);
  }

  /**
   * Set which direction the encoder simulates motion in.
   *
   * @param isForward Whether to simulate forward motion
   */
  public void setForward(boolean isForward) {
    m_forward = isForward;
  }

  /**
   * Accesses whether the encoder is simulating forward motion.
   *
   * @return Whether the simulated motion is in the forward direction
   */
  public boolean isForward() {
    return m_forward;
  }
}
