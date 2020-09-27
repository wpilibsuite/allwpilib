/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <Eigen/Core>
#include <units/current.h>
#include <units/time.h>

#include "frc/StateSpaceUtil.h"
#include "frc/system/LinearSystem.h"

namespace frc::sim {
/**
 * Represents a simulated generic linear system.
 */
template <int States, int Inputs, int Outputs>
class LinearSystemSim {
 public:
  /**
   * Creates a simulated generic linear system.
   *
   * @param system             The system to simulate.
   * @param addNoise           Whether the sim should automatically add some
   *                           measurement noise.
   * @param measurementStdDevs The standard deviations of the measurement noise.
   */
  LinearSystemSim(const LinearSystem<States, Inputs, Outputs>& system,
                  bool addNoise = false,
                  const std::array<double, Outputs>& measurementStdDevs = {0.0})
      : m_plant(system),
        m_shouldAddNoise(addNoise),
        m_measurementStdDevs(measurementStdDevs) {
    m_x = Eigen::Matrix<double, States, 1>::Zero();
    m_y = Eigen::Matrix<double, Outputs, 1>::Zero();
    m_u = Eigen::Matrix<double, Inputs, 1>::Zero();
  }

  /**
   * Returns whether the sim should add noise to the measurements.
   *
   * @return Whether the sim should add noise to the measurements.
   */
  bool ShouldAddNoise() const { return m_shouldAddNoise; }

  /**
   * Returns the current output of the plant.
   *
   * @return The current output of the plant.
   */
  const Eigen::Matrix<double, Outputs, 1>& Y() const { return m_y; }

  /**
   * Returns an element of the current output of the plant.
   *
   * @param row The row to return.
   */
  double Y(int i) const { return m_y(i); }

  /**
   * Sets whether the sim should add noise to measurements.
   *
   * @param shouldAddNoise Whether the sim should add noise to measurements.
   */
  void SetShouldAddNoise(bool shouldAddNoise) {
    m_shouldAddNoise = shouldAddNoise;
  }

  /**
   * Updates the linear system sim.
   *
   * @param dt The time between updates.
   */
  void Update(units::second_t dt) {
    // Update x. By default, this is the linear system dynamics x_k+1 = Ax_k +
    // Bu_k
    m_x = UpdateX(m_x, m_u, dt);

    // y = Cx + Du
    m_y = m_plant.CalculateY(m_x, m_u);

    // Add noise if needed.
    if (m_shouldAddNoise) {
      m_y += frc::MakeWhiteNoiseVector<Outputs>(m_measurementStdDevs);
    }
  }

  /**
   * Sets the system inputs (usually voltages).
   *
   * @param u The system inputs.
   */
  void SetInput(const Eigen::Matrix<double, Inputs, 1>& u) { m_u = u; }

  /**
   * Sets the system state.
   *
   * @param state The state.
   */
  void SetState(const Eigen::Matrix<double, States, 1>& state) { m_x = state; }

 protected:
  /**
   * Updates the state estimate of the system.
   *
   * @param currentXhat The current state estimate.
   * @param u           The system inputs (usually voltage).
   * @param dt          The time difference between controller updates.
   */
  virtual Eigen::Matrix<double, States, 1> UpdateX(
      const Eigen::Matrix<double, States, 1>& currentXhat,
      const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
    return m_plant.CalculateX(currentXhat, u, dt);
  }

  virtual units::ampere_t GetCurrentDraw() const {
    return units::ampere_t(0.0);
  }

  LinearSystem<States, Inputs, Outputs> m_plant;
  bool m_shouldAddNoise;

  Eigen::Matrix<double, States, 1> m_x;
  Eigen::Matrix<double, Outputs, 1> m_y;
  Eigen::Matrix<double, Inputs, 1> m_u;
  std::array<double, Outputs> m_measurementStdDevs;
};
}  // namespace frc::sim
