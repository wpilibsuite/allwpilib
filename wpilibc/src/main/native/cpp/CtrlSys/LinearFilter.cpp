/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CtrlSys/LinearFilter.h"

#include <cassert>
#include <cmath>

using namespace frc;

/**
 * Creates a one-pole IIR low-pass filter of the form:<br>
 *   y[n] = (1 - gain) * x[n] + gain * y[n-1]<br>
 * where gain = e<sup>-dt / T</sup>, T is the time constant in seconds
 *
 * This filter is stable for time constants greater than zero.
 *
 * @param SinglePoleIIR The tag for the type of filter to construct
 * @param source        The INode object that is used to get values
 * @param timeConstant  The discrete-time time constant in seconds
 * @param period        The period in seconds between samples taken by the user
 */
LinearFilter::LinearFilter(SinglePoleIIR, INode& input, double timeConstant,
                           double period)
    : NodeBase(input), m_inputs(1), m_outputs(1) {
  double gain = std::exp(-period / timeConstant);

  m_inputGains.emplace_back(1.0 - gain);
  m_outputGains.emplace_back(-gain);
}

/**
 * Creates a first-order high-pass filter of the form:<br>
 *   y[n] = gain * x[n] + (-gain) * x[n-1] + gain * y[n-1]<br>
 * where gain = e<sup>-dt / T</sup>, T is the time constant in seconds
 *
 * This filter is stable for time constants greater than zero.
 *
 * @param HighPass     The tag for the type of filter to construct
 * @param source       The INode object that is used to get values
 * @param timeConstant The discrete-time time constant in seconds
 * @param period       The period in seconds between samples taken by the user
 */
LinearFilter::LinearFilter(HighPass, INode& input, double timeConstant,
                           double period)
    : NodeBase(input), m_inputs(2), m_outputs(1) {
  double gain = std::exp(-period / timeConstant);

  m_inputGains.emplace_back(gain);
  m_inputGains.emplace_back(-gain);
  m_outputGains.emplace_back(-gain);
}

/**
 * Creates a K-tap FIR moving average filter of the form:<br>
 *   y[n] = 1/k * (x[k] + x[k-1] + … + x[0])
 *
 * This filter is always stable.
 *
 * @param MovingAverage The tag for the type of filter to construct
 * @param source        The INode object that is used to get values
 * @param taps          The number of samples to average over. Higher = smoother
 *                      but slower
 */
LinearFilter::LinearFilter(MovingAverage, INode& input, int taps)
    : NodeBase(input),
      m_inputs(1),
      m_outputs(0),
      m_inputGains(taps, 1.0 / taps) {
  assert(taps > 0);
}

/**
 * Create a linear FIR or IIR filter.
 *
 * @param input   The INode object that is used to get values
 * @param ffGains The "feed forward" or FIR gains
 * @param fbGains The "feed back" or IIR gains
 */
LinearFilter::LinearFilter(INode& input, llvm::ArrayRef<double> ffGains,
                           llvm::ArrayRef<double> fbGains)
    : NodeBase(input),
      m_inputs(ffGains.size()),
      m_outputs(fbGains.size()),
      m_inputGains(ffGains),
      m_outputGains(fbGains) {}

/**
 * Calculates the next value of the filter.
 *
 * @return The filtered value at this step
 */
double LinearFilter::GetOutput() {
  double retVal = 0.0;

  // Rotate the inputs
  m_inputs.push_front(NodeBase::GetOutput());

  // Calculate the new value
  for (size_t i = 0; i < m_inputGains.size(); i++) {
    retVal += m_inputs[i] * m_inputGains[i];
  }
  for (size_t i = 0; i < m_outputGains.size(); i++) {
    retVal -= m_outputs[i] * m_outputGains[i];
  }

  // Rotate the outputs
  m_outputs.push_front(retVal);

  return retVal;
}

/**
 * Resets filter state.
 *
 * Warning: This operation is not thread-safe. Only call this when the Output
 * instance using a graph containing this node is disabled.
 */
void LinearFilter::Reset() {
  m_inputs.reset();
  m_outputs.reset();
}
