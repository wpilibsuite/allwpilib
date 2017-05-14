/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.concurrent.locks.ReentrantLock;

public class GainNode extends NodeBase {
  private double m_gain;

  private ReentrantLock m_mutex = new ReentrantLock();

  /**
   * Returns the input node's output multiplied by a constant gain.
   *
   * @param K the gain on the input
   * @param input the input node
   */
  @SuppressWarnings("ParameterName")
  public GainNode(double K, INode input) {
    super(input);

    m_gain = K;
  }

  @Override
  public double getOutput() {
    m_mutex.lock();
    try {
      return m_gain * super.getOutput();
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Set gain applied to node output.
   *
   * @param K a gain to apply
   */
  @SuppressWarnings("ParameterName")
  public void setGain(double K) {
    m_mutex.lock();
    m_gain = K;
    m_mutex.unlock();
  }

  /**
   * Return gain applied to node output.
   */
  public double getGain() {
    m_mutex.lock();
    try {
      return m_gain;
    } finally {
      m_mutex.unlock();
    }
  }
}
