/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/DerivativeNode.h"

using namespace frc;

/**
 * Construct a differentiator.
 *
 * @param K a gain to apply to the differentiator output
 * @param input the input node
 * @param period the loop time for doing calculations.
 */
DerivativeNode::DerivativeNode(double K, INode& input, double period)
    : NodeBase(input) {
  m_gain = K;
  m_period = period;
}

double DerivativeNode::GetOutput() {
  double input = NodeBase::GetOutput();

  std::lock_guard<std::mutex> lock(m_mutex);

  double output = m_gain * (input - m_prevInput) / m_period;

  m_prevInput = input;

  return output;
}

/**
 * Set gain applied to node output.
 *
 * @param K a gain to apply
 */
void DerivativeNode::SetGain(double K) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_gain = K;
}

/**
 * Return gain applied to node output.
 */
double DerivativeNode::GetGain() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_gain;
}

/**
 * Clears derivative state.
 */
void DerivativeNode::Reset() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_prevInput = 0.0;
}
