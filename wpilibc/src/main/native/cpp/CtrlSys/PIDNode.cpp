/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/PIDNode.h"

using namespace frc;

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
PIDNode::PIDNode(double Kp, double Ki, double Kd, INode& input, double period)
    : NodeBase(input),
      m_P(Kp, input),
      m_I(Ki, input, period),
      m_D(Kd, input, period),
      m_sum(m_P, true, m_I, true, m_D, true) {}

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
PIDNode::PIDNode(double Kp, double Ki, double Kd, INode& feedforward,
                 INode& input, double period)
    : NodeBase(input),
      m_P(Kp, input),
      m_I(Ki, input, period),
      m_D(Kd, input, period),
      m_sum(m_P, true, m_I, true, m_D, true, feedforward, true) {}

double PIDNode::GetOutput() {
  double sum = m_sum.GetOutput();

  if (sum > m_maxU) {
    return m_maxU;
  } else if (sum < m_minU) {
    return m_minU;
  } else {
    return sum;
  }
}

/**
 * Set the PID Controller gain parameters. Set the proportional, integral, and
 * differential coefficients.
 *
 * @param p Proportional coefficient
 * @param i Integral coefficient
 * @param d Differential coefficient
 */
void PIDNode::SetPID(double p, double i, double d) {
  m_P.SetGain(p);
  m_I.SetGain(i);
  m_D.SetGain(d);
}

/**
 * Set the Proportional coefficient.
 *
 * @param p Proportional coefficient
 */
void PIDNode::SetP(double p) { m_P.SetGain(p); }

/**
 * Get the Proportional coefficient.
 *
 * @return proportional coefficient
 */
double PIDNode::GetP() const { return m_P.GetGain(); }

/**
 * Set the Integral coefficient.
 *
 * @param i Integral coefficient
 */
void PIDNode::SetI(double i) { m_I.SetGain(i); }

/**
 * Get the Integral coefficient.
 *
 * @return integral coefficient
 */
double PIDNode::GetI() const { return m_I.GetGain(); }

/**
 * Set the Differential coefficient.
 *
 * @param d Differential coefficient
 */
void PIDNode::SetD(double d) { m_D.SetGain(d); }

/**
 * Get the Differential coefficient.
 *
 * @return differential coefficient
 */
double PIDNode::GetD() const { return m_D.GetGain(); }

/**
 * Sets the minimum and maximum values to write.
 *
 * @param minU the minimum value to write to the output
 * @param maxU the maximum value to write to the output
 */
void PIDNode::SetOutputRange(double minU, double maxU) {
  m_minU = minU;
  m_maxU = maxU;
}

/**
 * Set maximum magnitude of input for which integration should occur. Values
 * above this will reset the current total.
 *
 * @param maxInputMagnitude max value of input for which integration should
 *                          occur
 */
void PIDNode::SetIZone(double maxInputMagnitude) {
  m_I.SetIZone(maxInputMagnitude);
}

/**
 * Clear the integral and derivative states.
 */
void PIDNode::Reset() {
  m_I.Reset();
  m_D.Reset();
}
