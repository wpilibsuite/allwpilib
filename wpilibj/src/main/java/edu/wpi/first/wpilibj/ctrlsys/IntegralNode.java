/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.concurrent.locks.ReentrantLock;

/**
 * Represents an integrator in a control system diagram.
 */
public class IntegralNode extends NodeBase {
  private double m_gain;
  private double m_period;

  private double m_total = 0.0;
  private double m_maxInputMagnitude = Double.POSITIVE_INFINITY;

  private ReentrantLock m_mutex = new ReentrantLock();

  /**
   * Construct an integrator.
   *
   * @param K a gain to apply to the integrator output
   * @param input the input node
   * @param period the loop time for doing calculations.
   */
  @SuppressWarnings("ParameterName")
  public IntegralNode(double K, INode input, double period) {
    super(input);

    m_gain = K;
    m_period = period;
  }

  /**
   * Construct an integrator.
   *
   * @param K a gain to apply to the integrator output
   * @param input the input node
   */
  @SuppressWarnings("ParameterName")
  public IntegralNode(double K, INode input) {
    this(K, input, INode.DEFAULT_PERIOD);
  }

  @Override
  public double getOutput() {
    double input = super.getOutput();

    m_mutex.lock();
    try {
      if (Math.abs(input) > m_maxInputMagnitude) {
        m_total = 0.0;
      } else {
        m_total = clamp(m_total + input * m_period, -1.0 / m_gain, 1.0 / m_gain);
      }

      return m_gain * m_total;
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
   * Set maximum magnitude of input for which integration should occur. Values above this will
   * reset the current total.
   *
   * @param maxInputMagnitude max value of input for which integration should occur
   */
  public void setIZone(double maxInputMagnitude) {
    m_mutex.lock();
    m_maxInputMagnitude = maxInputMagnitude;
    m_mutex.unlock();
  }

  /**
   * Clears integral state.
   */
  public void reset() {
    m_mutex.lock();
    m_total = 0.0;
    m_mutex.unlock();
  }

  private static double clamp(double value, double low, double high) {
    return Math.max(low, Math.min(value, high));
  }
}
