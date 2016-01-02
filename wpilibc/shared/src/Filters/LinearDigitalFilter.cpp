/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Filters/LinearDigitalFilter.h"
#include <cassert>
#include <cmath>

/**
 * Create a linear FIR or IIR filter
 *
 * @param source The PIDSource object that is used to get values
 * @param ffGains The "feed forward" or FIR gains
 * @param fbGains The "feed back" or IIR gains
 */
LinearDigitalFilter::LinearDigitalFilter(std::shared_ptr<PIDSource> source,
                                         std::initializer_list<double> ffGains,
                                         std::initializer_list<double> fbGains) :
    Filter(source), m_inputs(ffGains.size()), m_outputs(fbGains.size()),
    m_inputGains(ffGains), m_outputGains(fbGains) {}

/**
 * Create a linear FIR or IIR filter
 *
 * @param source The PIDSource object that is used to get values
 * @param ffGains The "feed forward" or FIR gains
 * @param fbGains The "feed back" or IIR gains
 */
LinearDigitalFilter::LinearDigitalFilter(std::shared_ptr<PIDSource> source,
                                         std::initializer_list<double> ffGains,
                                         const std::vector<double>& fbGains) :
    Filter(source), m_inputs(ffGains.size()), m_outputs(fbGains.size()),
    m_inputGains(ffGains), m_outputGains(fbGains) {}

/**
 * Create a linear FIR or IIR filter
 *
 * @param source The PIDSource object that is used to get values
 * @param ffGains The "feed forward" or FIR gains
 * @param fbGains The "feed back" or IIR gains
 */
LinearDigitalFilter::LinearDigitalFilter(std::shared_ptr<PIDSource> source,
                                         const std::vector<double>& ffGains,
                                         std::initializer_list<double> fbGains) :
    Filter(source), m_inputs(ffGains.size()), m_outputs(fbGains.size()),
    m_inputGains(ffGains), m_outputGains(fbGains) {}

/**
 * Create a linear FIR or IIR filter
 *
 * @param source The PIDSource object that is used to get values
 * @param ffGains The "feed forward" or FIR gains
 * @param fbGains The "feed back" or IIR gains
 */
LinearDigitalFilter::LinearDigitalFilter(std::shared_ptr<PIDSource> source,
                                         const std::vector<double>& ffGains,
                                         const std::vector<double>& fbGains) :
    Filter(source), m_inputs(ffGains.size()), m_outputs(fbGains.size()),
    m_inputGains(ffGains), m_outputGains(fbGains) {}

/**
 * Creates a one-pole IIR low-pass filter of the form:
 *   y[n] = (1-gain)*x[n] + gain*y[n-1]
 * where gain = e^(-dt / T), T is the time constant in seconds
 *
 * This filter is stable for time constants greater than zero
 *
 * @param source The PIDSource object that is used to get values
 * @param timeConstant The discrete-time time constant in seconds
 * @param period The period in seconds between samples taken by the user
 */
LinearDigitalFilter LinearDigitalFilter::SinglePoleIIR(std::shared_ptr<PIDSource> source,
                                                       double timeConstant,
                                                       double period) {
  double gain = std::exp(-period / timeConstant);
  return LinearDigitalFilter(std::move(source), {1.0 - gain}, {-gain});
}

/**
 * Creates a first-order high-pass filter of the form:
 *   y[n] = gain*x[n] + (-gain)*x[n-1] + gain*y[n-1]
 * where gain = e^(-dt / T), T is the time constant in seconds
 *
 * This filter is stable for time constants greater than zero
 *
 * @param source The PIDSource object that is used to get values
 * @param timeConstant The discrete-time time constant in seconds
 * @param period The period in seconds between samples taken by the user
 */
LinearDigitalFilter LinearDigitalFilter::HighPass(std::shared_ptr<PIDSource> source,
                                                  double timeConstant,
                                                  double period) {
  double gain = std::exp(-period / timeConstant);
  return LinearDigitalFilter(std::move(source), {gain, -gain}, {-gain});
}

/**
 * Creates a K-tap FIR moving average filter of the form:
 *   y[n] = 1/k * (x[k] + x[k-1] + ... + x[0])
 *
 * This filter is always stable.
 *
 * @param source The PIDSource object that is used to get values
 * @param taps The number of samples to average over. Higher = smoother but
 *        slower
 */
LinearDigitalFilter LinearDigitalFilter::MovingAverage(std::shared_ptr<PIDSource> source,
                                                       unsigned int taps) {
  assert(taps > 0);

  std::vector<double> gains(taps, 1.0 / taps);
  return LinearDigitalFilter(std::move(source), gains, {});
}

/**
 * {@inheritDoc}
 */
double LinearDigitalFilter::Get() const {
  double retVal = 0.0;

  // Calculate the new value
  for (unsigned int i = 0; i < m_inputGains.size(); i++) {
    retVal += m_inputs[i] * m_inputGains[i];
  }
  for (unsigned int i = 0; i < m_outputGains.size(); i++) {
    retVal -= m_outputs[i] * m_outputGains[i];
  }

  return retVal;
}

/**
 * {@inheritDoc}
 */
void LinearDigitalFilter::Reset() {
  m_inputs.Reset();
  m_outputs.Reset();
}

/**
 * Calculates the next value of the filter
 *
 * @return The filtered value at this step
 */
double LinearDigitalFilter::PIDGet() {
  double retVal = 0.0;

  // Rotate the inputs
  m_inputs.PushFront(PIDGetSource());

  // Calculate the new value
  for (unsigned int i = 0; i < m_inputGains.size(); i++) {
    retVal += m_inputs[i] * m_inputGains[i];
  }
  for (unsigned int i = 0; i < m_outputGains.size(); i++) {
    retVal -= m_outputs[i] * m_outputGains[i];
  }

  // Rotate the outputs
  m_outputs.PushFront(retVal);

  return retVal;
}
