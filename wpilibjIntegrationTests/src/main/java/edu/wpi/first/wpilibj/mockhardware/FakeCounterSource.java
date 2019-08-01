// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.mockhardware;

import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.Timer;

/** Simulates an encoder for testing purposes. */
public class FakeCounterSource implements AutoCloseable {
  private Thread m_task;
  private int m_count;
  private int m_milliSec;
  private DigitalOutput m_output;
  private boolean m_allocated;

  /** Thread object that allows emulation of an encoder. */
  private static class EncoderThread extends Thread {
    FakeCounterSource m_encoder;

    EncoderThread(FakeCounterSource encode) {
      m_encoder = encode;
    }

    @Override
    public void run() {
      m_encoder.m_output.set(false);
      try {
        for (int i = 0; i < m_encoder.m_count; i++) {
          Thread.sleep(m_encoder.m_milliSec);
          m_encoder.m_output.set(true);
          Thread.sleep(m_encoder.m_milliSec);
          m_encoder.m_output.set(false);
        }
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    }
  }

  /**
   * Create a fake encoder on a given port.
   *
   * @param output the port to output the given signal to
   */
  public FakeCounterSource(DigitalOutput output) {
    m_output = output;
    m_allocated = false;
    initEncoder();
  }

  /**
   * Create a fake encoder on a given port.
   *
   * @param port The port the encoder is supposed to be on
   */
  public FakeCounterSource(int port) {
    m_output = new DigitalOutput(port);
    m_allocated = true;
    initEncoder();
  }

  /** Destroy Object with minimum memory leak. */
  @Override
  public void close() {
    m_task = null;
    if (m_allocated) {
      m_output.close();
      m_output = null;
      m_allocated = false;
    }
  }

  /** Common initailization code. */
  private void initEncoder() {
    m_milliSec = 1;
    m_task = new EncoderThread(this);
    m_output.set(false);
  }

  /** Starts the thread execution task. */
  public void start() {
    m_task.start();
  }

  /** Waits for the thread to complete. */
  public void complete() {
    try {
      m_task.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    m_task = new EncoderThread(this);
    Timer.delay(0.01);
  }

  /** Starts and completes a task set - does not return until thred has finished its operations. */
  public void execute() {
    start();
    complete();
  }

  /**
   * Sets the count to run encoder.
   *
   * @param count The count to emulate to the controller
   */
  public void setCount(int count) {
    m_count = count;
  }

  /**
   * Specify the rate to send pulses.
   *
   * @param milliSec The rate to send out pulses at
   */
  public void setRate(int milliSec) {
    m_milliSec = milliSec;
  }
}
