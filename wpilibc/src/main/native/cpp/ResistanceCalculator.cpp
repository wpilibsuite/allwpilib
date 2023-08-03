// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ResistanceCalculator.h"

#include <cmath>

using namespace frc;

ResistanceCalculator::ResistanceCalculator(int bufferSize,
                                           double rSquaredThreshold)
    : m_currentBuffer(bufferSize),
      m_voltageBuffer(bufferSize),
      m_bufferSize{bufferSize},
      m_rSquaredThreshold{rSquaredThreshold} {}

std::optional<units::ohm_t> ResistanceCalculator::Calculate(
    units::ampere_t current, units::volt_t voltage) {
  // Update buffers only if drawing current
  if (current != 0_A) {
    if (m_currentBuffer.size() >= static_cast<size_t>(m_bufferSize)) {
      // Pop the last point and remove it from the sums
      units::ampere_t lastCurrent = m_currentBuffer.pop_back();
      units::volt_t lastVoltage = m_voltageBuffer.pop_back();
      m_currentVariance.Calculate(lastCurrent, lastCurrent, Operator::kRemove);
      m_voltageVariance.Calculate(lastVoltage, lastVoltage, Operator::kRemove);
      m_covariance.Calculate(lastCurrent, lastVoltage, Operator::kRemove);
    }

    m_currentBuffer.push_front(current);
    m_voltageBuffer.push_front(voltage);
    m_currentVariance.Calculate(current, current, Operator::kAdd);
    m_voltageVariance.Calculate(voltage, voltage, Operator::kAdd);
    m_covariance.Calculate(current, voltage, Operator::kAdd);
  }

  // Recalculate resistance
  if (m_currentBuffer.size() < 2) {
    return std::nullopt;
  }

  auto currentVariance = m_currentVariance.GetCovariance();
  auto voltageVariance = m_voltageVariance.GetCovariance();
  auto covariance = m_covariance.GetCovariance();
  double rSquared =
      covariance * covariance / (currentVariance * voltageVariance);

  if (rSquared > m_rSquaredThreshold) {
    // Resistance is slope of current vs voltage
    return covariance / currentVariance;
  } else {
    return std::nullopt;
  }
}
