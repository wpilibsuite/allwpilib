/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/filters/LinearDigitalFilter.h"

#include <cassert>
#include <cmath>

#include <hal/HAL.h>

using namespace frc;

LinearDigitalFilter::LinearDigitalFilter(PIDSource& source,
                                         wpi::ArrayRef<double> ffGains,
                                         wpi::ArrayRef<double> fbGains)
    : Filter(source),
      m_inputs(ffGains.size()),
      m_outputs(fbGains.size()),
      m_inputGains(ffGains),
      m_outputGains(fbGains) {
  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_LinearFilter, instances);
}

LinearDigitalFilter::LinearDigitalFilter(std::shared_ptr<PIDSource> source,
                                         wpi::ArrayRef<double> ffGains,
                                         wpi::ArrayRef<double> fbGains)
    : Filter(source),
      m_inputs(ffGains.size()),
      m_outputs(fbGains.size()),
      m_inputGains(ffGains),
      m_outputGains(fbGains) {
  static int instances = 0;
  instances++;
  HAL_Report(HALUsageReporting::kResourceType_LinearFilter, instances);
}

LinearDigitalFilter LinearDigitalFilter::SinglePoleIIR(PIDSource& source,
                                                       double timeConstant,
                                                       double period) {
  double gain = std::exp(-period / timeConstant);
  return LinearDigitalFilter(source, {1.0 - gain}, {-gain});
}

LinearDigitalFilter LinearDigitalFilter::HighPass(PIDSource& source,
                                                  double timeConstant,
                                                  double period) {
  double gain = std::exp(-period / timeConstant);
  return LinearDigitalFilter(source, {gain, -gain}, {-gain});
}

LinearDigitalFilter LinearDigitalFilter::MovingAverage(PIDSource& source,
                                                       int taps) {
  assert(taps > 0);

  std::vector<double> gains(taps, 1.0 / taps);
  return LinearDigitalFilter(source, gains, {});
}

LinearDigitalFilter LinearDigitalFilter::SinglePoleIIR(
    std::shared_ptr<PIDSource> source, double timeConstant, double period) {
  double gain = std::exp(-period / timeConstant);
  return LinearDigitalFilter(std::move(source), {1.0 - gain}, {-gain});
}

LinearDigitalFilter LinearDigitalFilter::HighPass(
    std::shared_ptr<PIDSource> source, double timeConstant, double period) {
  double gain = std::exp(-period / timeConstant);
  return LinearDigitalFilter(std::move(source), {gain, -gain}, {-gain});
}

LinearDigitalFilter LinearDigitalFilter::MovingAverage(
    std::shared_ptr<PIDSource> source, int taps) {
  assert(taps > 0);

  std::vector<double> gains(taps, 1.0 / taps);
  return LinearDigitalFilter(std::move(source), gains, {});
}

double LinearDigitalFilter::Get() const {
  double retVal = 0.0;

  // Calculate the new value
  for (size_t i = 0; i < m_inputGains.size(); i++) {
    retVal += m_inputs[i] * m_inputGains[i];
  }
  for (size_t i = 0; i < m_outputGains.size(); i++) {
    retVal -= m_outputs[i] * m_outputGains[i];
  }

  return retVal;
}

void LinearDigitalFilter::Reset() {
  m_inputs.reset();
  m_outputs.reset();
}

double LinearDigitalFilter::PIDGet() {
  double retVal = 0.0;

  // Rotate the inputs
  m_inputs.push_front(PIDGetSource());

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
