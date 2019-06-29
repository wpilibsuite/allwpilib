/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/LinearFilter.h"

#include <cassert>
#include <cmath>

using namespace frc;

LinearFilter::LinearFilter(wpi::ArrayRef<double> ffGains,
                           wpi::ArrayRef<double> fbGains)
    : m_inputs(ffGains.size()),
      m_outputs(fbGains.size()),
      m_inputGains(ffGains),
      m_outputGains(fbGains) {}

LinearFilter LinearFilter::SinglePoleIIR(double timeConstant, double period) {
  double gain = std::exp(-period / timeConstant);
  return LinearFilter(1.0 - gain, -gain);
}

LinearFilter LinearFilter::HighPass(double timeConstant, double period) {
  double gain = std::exp(-period / timeConstant);
  const double ffGains[] = {gain, -gain};
  return LinearFilter(ffGains, -gain);
}

LinearFilter LinearFilter::MovingAverage(int taps) {
  assert(taps > 0);

  std::vector<double> gains(taps, 1.0 / taps);
  return LinearFilter(gains, {});
}

void LinearFilter::Reset() {
  m_inputs.reset();
  m_outputs.reset();
}

double LinearFilter::Calculate(double input) {
  double retVal = 0.0;

  // Rotate the inputs
  m_inputs.push_front(input);

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
