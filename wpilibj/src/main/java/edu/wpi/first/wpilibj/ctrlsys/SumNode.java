/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

import java.util.concurrent.locks.ReentrantLock;

/**
 * Takes an arbitrary list of input nodes and outputs the sum of their outputs.
 */
public class SumNode implements INode {
  private Input[] m_inputs;

  private double m_currentResult = 0.0;
  private double m_lastResult = 0.0;

  private boolean m_continuous = false;
  private double m_inputRange = 0.0;

  private double m_tolerance = Double.POSITIVE_INFINITY;
  private double m_deltaTolerance = Double.POSITIVE_INFINITY;

  private ReentrantLock m_mutex = new ReentrantLock();

  private class Input {
    public INode m_input;
    public boolean m_positive;

    Input(INode input, boolean positive) {
      m_input = input;
      m_positive = positive;
    }
  }

  /**
   * Appends input and associated sign to array.
   *
   * @param input    The INode object to add to the array for round robin.
   * @param positive If true, adds input; otherwise, subtracts input.
   * @param inputs   The other INode objects and booleans.
   */
  public SumNode(INode input, boolean positive, Object... inputs) {
    m_inputs = new Input[1 + inputs.length / 2];

    m_inputs[0] = new Input(input, positive);
    for (int i = 0; i < inputs.length; i += 2) {
      m_inputs[1 + i / 2] = new Input((INode) inputs[i], (boolean) inputs[i + 1]);
    }
  }

  @Override
  public double getOutput() {
    double sum = 0.0;

    for (Input input : m_inputs) {
      if (input.m_positive) {
        sum += input.m_input.getOutput();
      } else {
        sum -= input.m_input.getOutput();
      }
    }

    m_mutex.lock();

    try {
      m_lastResult = m_currentResult;
      m_currentResult = sum;

      if (m_continuous && m_inputRange > 0) {
        sum %= m_inputRange;
        if (Math.abs(sum) > m_inputRange / 2.0) {
          if (sum > 0.0) {
            return sum - m_inputRange;
          } else {
            return sum + m_inputRange;
          }
        }
      }

      return sum;
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Set the sum node to consider the input to be continuous.
   *
   * <p>Rather then using the max and min in as constraints, it considers them to be the same point
   * and automatically calculates the shortest route to the reference (e.g., gyroscope angle).
   *
   * @param continuous true turns on continuous; false turns off continuous
   */
  public void setContinuous(boolean continuous) {
    m_mutex.lock();
    m_continuous = continuous;
    m_mutex.unlock();
  }

  /**
   * Set the sum node to consider the input to be continuous.
   *
   * <p>Rather then using the max and min in as constraints, it considers them to be the same point
   * and automatically calculates the shortest route to the reference (e.g., gyroscope angle).
   */
  public void setContinuous() {
    setContinuous(true);
  }

  /**
   * Sets the maximum and minimum values expected from the input.
   *
   * @param minimumInput the minimum value expected from the input
   * @param maximumInput the maximum value expected from the input
   */
  public void setInputRange(double minimumInput, double maximumInput) {
    m_mutex.lock();
    m_inputRange  = maximumInput - minimumInput;
    m_mutex.unlock();
  }

  /**
   * Set the absolute error which is considered tolerable for use with OnTarget.
   *
   * @param tolerance absolute error which is tolerable
   * @param deltaTolerance change in absolute error which is tolerable
   */
  public void setTolerance(double tolerance, double deltaTolerance) {
    m_mutex.lock();
    m_tolerance = tolerance;
    m_deltaTolerance = deltaTolerance;
    m_mutex.unlock();
  }

  /**
   * Return true if the error and change in error is within the range determined by setTolerance().
   */
  public boolean inTolerance() {
    m_mutex.lock();
    try {
      return Math.abs(m_currentResult) < m_tolerance
          && Math.abs(m_currentResult - m_lastResult) < m_deltaTolerance;
    } finally {
      m_mutex.unlock();
    }
  }
}
