// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/ResistanceCalculator.h"

#include <cmath>

#include <units/math.h>

using namespace frc;

/** The previously calculated covariance. */
double ResistanceCalculator::OnlineCovariance::GetCovariance() {
  return m_cov / (m_n - 1);
}

/**
 * Calculate the covariance based on a new point that may be removed or added.
 * @param x The x value of the point.
 * @param y The y value of the point.
 * @param remove Whether to remove the point or add it.
 * @return The new sample covariance.
 */
double ResistanceCalculator::OnlineCovariance::Calculate(double x, double y,
                                                         bool remove) {
  // From
  // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Covariance
  auto factor = remove ? -1 : 1;
  m_n += factor;

  auto dx = x - m_xMean;
  auto dy = y - m_yMean;

  m_xMean += factor * dx / m_n;
  m_yMean += factor * dy / m_n;

  // This is supposed to be (y - yMean) and not dy
  m_cov += factor * dx * (y - m_yMean);

  // Correction for sample variance
  return m_cov / (m_n - 1);
}

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
      auto lastCurrent = m_currentBuffer.pop_back();
      auto lastVoltage = m_voltageBuffer.pop_back();
      m_currentVariance.Calculate(lastCurrent, lastCurrent, true);
      m_voltageVariance.Calculate(lastVoltage, lastVoltage, true);
      m_covariance.Calculate(lastCurrent, lastVoltage, true);
    } else {
      m_numPoints++;
    }

    m_currentBuffer.push_front(current());
    m_voltageBuffer.push_front(voltage());
    m_currentVariance.Calculate(current(), current(), false);
    m_voltageVariance.Calculate(voltage(), voltage(), false);
    m_covariance.Calculate(current(), voltage(), false);
  }

  // Recalculate resistance
  if (m_numPoints < 2) {
    return units::ohm_t{std::nan("")};
  }

  auto currentVariance = m_currentVariance.GetCovariance();
  auto voltageVariance = m_voltageVariance.GetCovariance();
  auto covariance = m_covariance.GetCovariance();
  auto rSquared = covariance * covariance / (currentVariance * voltageVariance);

  if (rSquared > m_rSquaredThreshold) {
    // Resistance is slope of current vs voltage negated
    auto slope = covariance / currentVariance;
    return units::ohm_t{-slope};
  } else {
    return units::ohm_t{std::nan("")};
  }
}
