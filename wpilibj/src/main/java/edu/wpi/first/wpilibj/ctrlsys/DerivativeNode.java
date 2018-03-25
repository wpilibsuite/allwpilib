/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.concurrent.locks.ReentrantLock;

/**
 * Returns the integral of the input node's output.
 */
public class DerivativeNode extends NodeBase {
  private double m_gain;
  private double m_period;

  private double m_prevInput = 0.0;

  private ReentrantLock m_mutex = new ReentrantLock();

  /**
   * Construct a differentiator.
   *
   * @param K a gain to apply to the differentiator output
   * @param input the input node
   * @param period the loop time for doing calculations.
   */
  @SuppressWarnings("ParameterName")
  public DerivativeNode(double K, INode input, double period) {
    super(input);

    m_gain = K;
    m_period = period;
  }

  /**
   * Construct a differentiator.
   *
   * @param K a gain to apply to the differentiator output
   * @param input the input node
   */
  @SuppressWarnings("ParameterName")
  public DerivativeNode(double K, INode input) {
    this(K, input, INode.DEFAULT_PERIOD);
  }

  @Override
  public double getOutput() {
    double input = super.getOutput();

    m_mutex.lock();
    try {
      double output = m_gain * (input - m_prevInput) / m_period;

      m_prevInput = input;

      return output;
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

  /**
   * Clears derivative state.
   */
  public void reset() {
    m_mutex.lock();
    m_prevInput = 0.0;
    m_mutex.unlock();
  }
}
