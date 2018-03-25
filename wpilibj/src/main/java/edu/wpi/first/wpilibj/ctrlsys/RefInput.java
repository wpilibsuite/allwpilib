/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.concurrent.locks.ReentrantLock;

/**
 * A node for reference inputs (e.g., setpoints).
 */
public class RefInput implements INode {
  private double m_reference;
  private Runnable m_callback = null;

  private ReentrantLock m_mutex = new ReentrantLock();

  public RefInput(double reference) {
    set(reference);
  }

  public RefInput() {
    this(0.0);
  }

  @Override
  public void setCallback(Output output) {
    m_callback = new Runnable() {
      public void run() {
        output.outputFunc();
      }
    };
  }

  /**
   * Returns value of reference input.
   */
  @Override
  public double getOutput() {
    m_mutex.lock();
    try {
      return m_reference;
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Sets reference input.
   */
  public void set(double reference) {
    m_mutex.lock();
    m_reference = reference;
    m_mutex.unlock();

    if (m_callback != null) {
      m_callback.run();
    }
  }
}
