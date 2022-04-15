// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ResistanceCalculator.h"

#include <cmath>

#include <units/math.h>

using namespace frc;

ResistanceCalculator::ResistanceCalculator(int bufferSize,
                                           double rSquaredThreshold)
    : m_currentBuffer(bufferSize),
      m_voltageBuffer(bufferSize),
      m_bufferSize(bufferSize),
      m_rSquaredThreshold(rSquaredThreshold) {}

ResistanceCalculator::ResistanceCalculator()
    : m_currentBuffer(ResistanceCalculator::kDefaultBufferSize),
      m_voltageBuffer(ResistanceCalculator::kDefaultBufferSize),
      m_bufferSize(ResistanceCalculator::kDefaultBufferSize),
      m_rSquaredThreshold(kDefaultRSquaredThreshold) {}

units::ohm_t ResistanceCalculator::Calculate(units::ampere_t current,
                                             units::volt_t voltage) {
  // Update buffers only if drawing current
  if (current != 0_A) {
    if (m_numPoints >= m_bufferSize) {
      // Pop the last point and remove it from the sums
      auto backCurrent = m_currentBuffer.pop_back();
      auto backVoltage = m_voltageBuffer.pop_back();
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
    return units::ohm_t{std::nan("")};
  }

  auto currentMean = m_currentSum / m_numPoints;
  auto voltageMean = m_voltageSum / m_numPoints;
  // todo make compound units work instead of turning into doubles
  auto currentVariance =
      (m_currentSquaredSum / m_numPoints) - currentMean * currentMean;
  auto voltageVariance =
      (m_voltageSquaredSum / m_numPoints) - voltageMean * voltageMean;
  auto covariance = (m_prodSum - m_currentSum * m_voltageSum / m_numPoints) /
                    (m_numPoints - 1);
  auto rSquared =
      covariance * covariance / (currentVariance * voltageVariance);

  if (rSquared > m_rSquaredThreshold) {
    // Slope of current vs voltage
    auto slope = covariance / currentVariance;
    return units::ohm_t{-slope};
  } else {
    return units::ohm_t{std::nan("")};
  }
}
