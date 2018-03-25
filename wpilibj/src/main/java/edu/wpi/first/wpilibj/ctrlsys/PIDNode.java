/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.ctrlsys;

/**
 * A PID controller implementation based on the control system diagram framework. This is more
 * general than the PIDController as its output can be chained with other nodes that are not
 * actuators.
 *
 * <p>For a simple closed-loop PID controller, see PIDController.
 */
public class PIDNode extends NodeBase {
  @SuppressWarnings("MemberName")
  private GainNode m_P;

  @SuppressWarnings("MemberName")
  private IntegralNode m_I;

  @SuppressWarnings("MemberName")
  private DerivativeNode m_D;

  private SumNode m_sum;

  private double m_minU = -1.0;
  private double m_maxU = 1.0;

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp the proportional coefficient
   * @param Ki the integral coefficient
   * @param Kd the derivative coefficient
   * @param input the node that is used to get values
   * @param period the loop time for doing calculations. This particularly
   *               effects calculations of the integral and differental terms.
   */
  @SuppressWarnings("ParameterName")
  public PIDNode(double Kp, double Ki, double Kd, INode input, double period) {
    super(input);

    m_P = new GainNode(Kp, input);
    m_I = new IntegralNode(Ki, input, period);
    m_D = new DerivativeNode(Kd, input, period);
    m_sum = new SumNode(m_P, true, m_I, true, m_D, true);
  }

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp the proportional coefficient
   * @param Ki the integral coefficient
   * @param Kd the derivative coefficient
   * @param input the node that is used to get values
   */
  @SuppressWarnings("ParameterName")
  public PIDNode(double Kp, double Ki, double Kd, INode input) {
    this(Kp, Ki, Kd, input, DEFAULT_PERIOD);
  }

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp the proportional coefficient
   * @param Ki the integral coefficient
   * @param Kd the derivative coefficient
   * @param feedforward node to use for feedforward calculations
   * @param input the node that is used to get values
   * @param period the loop time for doing calculations. This particularly
   *               effects calculations of the integral and differental terms.
   */
  @SuppressWarnings("ParameterName")
  public PIDNode(double Kp, double Ki, double Kd, INode feedforward, INode input,
          double period) {
    super(input);

    m_P = new GainNode(Kp, input);
    m_I = new IntegralNode(Ki, input, period);
    m_D = new DerivativeNode(Kd, input, period);
    m_sum = new SumNode(m_P, true, m_I, true, m_D, true, feedforward, true);
  }

  /**
   * Allocate a PID object with the given constants for P, I, D.
   *
   * @param Kp the proportional coefficient
   * @param Ki the integral coefficient
   * @param Kd the derivative coefficient
   * @param feedforward node to use for feedforward calculations
   * @param input the node that is used to get values
   */
  @SuppressWarnings("ParameterName")
  public PIDNode(double Kp, double Ki, double Kd, INode feedforward, INode input) {
    this(Kp, Ki, Kd, feedforward, input, DEFAULT_PERIOD);
  }

  @Override
  public double getOutput() {
    double sum = m_sum.getOutput();

    if (sum > m_maxU) {
      return m_maxU;
    } else if (sum < m_minU) {
      return m_minU;
    } else {
      return sum;
    }
  }

  /**
   * Set the PID Controller gain parameters. Set the proportional, integral, and differential
   * coefficients.
   *
   * @param p Proportional coefficient
   * @param i Integral coefficient
   * @param d Differential coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setPID(double p, double i, double d) {
    m_P.setGain(p);
    m_I.setGain(i);
    m_D.setGain(d);
  }

  /**
   * Set the Proportional coefficient.
   *
   * @param p Proportional coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setP(double p) {
    m_P.setGain(p);
  }

  /**
   * Get the Proportional coefficient.
   *
   * @return proportional coefficient
   */
  public double getP() {
    return m_P.getGain();
  }

  /**
   * Set the Integral coefficient.
   *
   * @param i Integral coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setI(double i) {
    m_I.setGain(i);
  }

  /**
   * Get the Integral coefficient.
   *
   * @return integral coefficient
   */
  public double getI() {
    return m_I.getGain();
  }

  /**
   * Set the Differential coefficient.
   *
   * @param d Differential coefficient
   */
  @SuppressWarnings("ParameterName")
  public void setD(double d) {
    m_D.setGain(d);
  }

  /**
   * Get the Differential coefficient.
   *
   * @return differential coefficient
   */
  public double getD() {
    return m_D.getGain();
  }

  /**
   * Sets the minimum and maximum values to write.
   *
   * @param minU the minimum value to write to the output
   * @param maxU the maximum value to write to the output
   */
  public void setOutputRange(double minU, double maxU) {
    m_minU = minU;
    m_maxU = maxU;
  }

  /**
   * Set maximum magnitude of input for which integration should occur. Values above this will
   * reset the current total.
   *
   * @param maxInputMagnitude max value of input for which integration should occur
   */
  public void setIZone(double maxInputMagnitude) {
    m_I.setIZone(maxInputMagnitude);
  }

  /**
   * Clear the integral and derivative states.
   */
  public void reset() {
    m_I.reset();
    m_D.reset();
  }
}
