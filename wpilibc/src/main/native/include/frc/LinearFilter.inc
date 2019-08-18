/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cassert>
#include <cmath>
#include <vector>

#include <hal/HAL.h>

namespace frc {

template <typename T>
LinearFilter<T>::LinearFilter(wpi::ArrayRef<double> ffGains,
                              wpi::ArrayRef<double> fbGains)
    : m_inputs(ffGains.size()),
      m_outputs(fbGains.size()),
      m_inputGains(ffGains),
      m_outputGains(fbGains) {
  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_LinearFilter, instances);
}

template <typename T>
LinearFilter<T> LinearFilter<T>::SinglePoleIIR(double timeConstant,
                                               units::second_t period) {
  double gain = std::exp(-period.to<double>() / timeConstant);
  return LinearFilter<T>(1.0 - gain, -gain);
}

template <typename T>
LinearFilter<T> LinearFilter<T>::HighPass(double timeConstant,
                                          units::second_t period) {
  double gain = std::exp(-period.to<double>() / timeConstant);
  return LinearFilter<T>({gain, -gain}, {-gain});
}

template <typename T>
LinearFilter<T> LinearFilter<T>::MovingAverage(int taps) {
  assert(taps > 0);

  std::vector<double> gains(taps, 1.0 / taps);
  return LinearFilter<T>(gains, {});
}

template <typename T>
void LinearFilter<T>::Reset() {
  m_inputs.reset();
  m_outputs.reset();
}

template <typename T>
T LinearFilter<T>::Calculate(T input) {
  T retVal{0.0};

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

}  // namespace frc
