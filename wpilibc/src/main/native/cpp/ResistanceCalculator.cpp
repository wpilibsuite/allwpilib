// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ResistanceCalculator.h"

#include <cmath>

using namespace frc;

ResistanceCalculator::ResistanceCalculator(int bufferSize,
                                           double krSquaredThreshold)
    : m_currentBuffer(bufferSize),
      m_voltageBuffer(bufferSize),
      m_bufferSize(bufferSize),
      m_rSquaredThreshold(krSquaredThreshold) {}

ResistanceCalculator::ResistanceCalculator()
    : m_currentBuffer(ResistanceCalculator::kDefaultBufferSize),
      m_voltageBuffer(ResistanceCalculator::kDefaultBufferSize),
      m_bufferSize(ResistanceCalculator::kDefaultBufferSize),
      m_rSquaredThreshold(kDefaultRSquaredThreshold) {}

double ResistanceCalculator::Calculate(double current, double voltage) {
  // Update buffers only if drawing current
  if (current != 0) {
    if (m_numPoints >= m_bufferSize) {
      // Pop the last point and remove it from the sums
      double backCurrent = m_currentBuffer.pop_back();
      double backVoltage = m_voltageBuffer.pop_back();
      m_currentSum -= backCurrent;
      m_voltageSum -= backVoltage;
      m_currentSquaredSum -= backCurrent * backCurrent;
      m_voltageSquaredSum -= backVoltage * backVoltage;
      m_prodSum -= backCurrent * backVoltage;
    } else {
      m_numPoints++;
    }
    m_currentBuffer.push_front(current);
    m_voltageBuffer.push_front(voltage);
    m_currentSum += current;
    m_voltageSum += voltage;
    m_currentSquaredSum += current * current;
    m_voltageSquaredSum += voltage * voltage;
    m_prodSum += current * voltage;
  }

  // Recalculate resistance
  if (m_numPoints < 2) {
    return std::nan("");
  }

  double currentMean = m_currentSum / m_numPoints;
  double voltageMean = m_voltageSum / m_numPoints;
  double currentVariance =
      (m_currentSquaredSum / m_numPoints) - currentMean * currentMean;
  double voltageVariance =
      (m_voltageSquaredSum / m_numPoints) - voltageMean * voltageMean;
  double covariance = (m_prodSum - m_currentSum * m_voltageSum / m_numPoints) /
                      (m_numPoints - 1);
  double krSquared =
      covariance * covariance / (currentVariance * voltageVariance);

  if (krSquared > m_rSquaredThreshold) {
    // Slope of current vs voltage
    double slope = covariance / currentVariance;
    return -slope;
  } else {
    return std::nan("");
  }
}
