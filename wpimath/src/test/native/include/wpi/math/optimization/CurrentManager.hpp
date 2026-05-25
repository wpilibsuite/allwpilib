// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <limits>
#include <span>
#include <stdexcept>
#include <vector>

#include <sleipnir/optimization/problem.hpp>

namespace wpi::math {

/**
 * This class computes the optimal current allocation for a list of subsystems
 * given a list of their desired currents and current tolerances that determine
 * which subsystem gets less current if the current budget is exceeded.
 * Subsystems with a smaller tolerance are given higher priority.
 */
class CurrentManager {
 public:
  /**
   * Constructs a CurrentManager.
   *
   * @param currentTolerances The relative current tolerance of each subsystem.
   * @param maxCurrent The current budget to allocate between subsystems.
   */
  CurrentManager(std::span<const double> currentTolerances, double maxCurrent)
      : m_desiredCurrents{static_cast<int>(currentTolerances.size()), 1},
        m_allocatedCurrents{
            m_problem.decision_variable(currentTolerances.size())} {
    // Ensure m_desiredCurrents contains initialized Variables
    for (int row = 0; row < m_desiredCurrents.rows(); ++row) {
      // Don't initialize to 0 or 1, because those will get folded by Sleipnir
      m_desiredCurrents[row] = std::numeric_limits<double>::infinity();
    }

    slp::Variable J = 0.0;
    slp::Variable current_sum = 0.0;
    for (size_t i = 0; i < currentTolerances.size(); ++i) {
      // The weight is 1/tolᵢ² where tolᵢ is the tolerance between the desired
      // and allocated current for subsystem i
      auto error = m_desiredCurrents[i] - m_allocatedCurrents[i];
      J += error * error / (currentTolerances[i] * currentTolerances[i]);

      current_sum += m_allocatedCurrents[i];

      // Currents must be nonnegative
      m_problem.subject_to(m_allocatedCurrents[i] >= 0.0);
    }
    m_problem.minimize(J);

    // Keep total current below maximum
    m_problem.subject_to(current_sum <= maxCurrent);
  }

  /**
   * Returns the optimal current allocation for a list of subsystems given a
   * list of their desired currents and current tolerances that determine which
   * subsystem gets less current if the current budget is exceeded. Subsystems
   * with a smaller tolerance are given higher priority.
   *
   * @param desiredCurrents The desired current for each subsystem.
   * @throws std::runtime_error if the number of desired currents doesn't equal
   *         the number of tolerances passed in the constructor.
   */
  std::vector<double> calculate(std::span<const double> desiredCurrents) {
    if (m_desiredCurrents.rows() != static_cast<int>(desiredCurrents.size())) {
      throw std::runtime_error(
          "Number of desired currents must equal the number of tolerances "
          "passed in the constructor.");
    }

    for (size_t i = 0; i < desiredCurrents.size(); ++i) {
      m_desiredCurrents[i].set_value(desiredCurrents[i]);
    }

    m_problem.solve();

    std::vector<double> result;
    for (size_t i = 0; i < desiredCurrents.size(); ++i) {
      result.emplace_back(std::max(m_allocatedCurrents.value(i), 0.0));
    }

    return result;
  }

 private:
  slp::Problem<double> m_problem;
  slp::VariableMatrix<double> m_desiredCurrents;
  slp::VariableMatrix<double> m_allocatedCurrents;
};

}  // namespace wpi::math
