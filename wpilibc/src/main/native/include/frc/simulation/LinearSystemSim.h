// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>

#include <Eigen/Core>
#include <units/current.h>
#include <units/time.h>

#include "frc/RobotController.h"
#include "frc/StateSpaceUtil.h"
#include "frc/system/LinearSystem.h"

namespace frc::sim {
/**
 * This class helps simulate linear systems. To use this class, do the following
 * in the simulationPeriodic() method.
 *
 * Call the SetInput() method with the inputs to your system (generally
 * voltage). Call the Update() method to update the simulation. Set simulated
 * sensor readings with the simulated positions in the GetOutput() method.
 *
 * @tparam States  The number of states of the system.
 * @tparam Inputs  The number of inputs to the system.
 * @tparam Outputs The number of outputs of the system.
 */
template <int States, int Inputs, int Outputs>
class LinearSystemSim {
 public:
  /**
   * Creates a simulated generic linear system.
   *
   * @param system             The system to simulate.
   * @param measurementStdDevs The standard deviations of the measurements.
   */
  explicit LinearSystemSim(
      const LinearSystem<States, Inputs, Outputs>& system,
      const std::array<double, Outputs>& measurementStdDevs = {})
      : m_plant(system), m_measurementStdDevs(measurementStdDevs) {
    m_x = Eigen::Matrix<double, States, 1>::Zero();
    m_y = Eigen::Matrix<double, Outputs, 1>::Zero();
    m_u = Eigen::Matrix<double, Inputs, 1>::Zero();
  }

  /**
   * Updates the simulation.
   *
   * @param dt The time between updates.
   */
  void Update(units::second_t dt) {
    // Update x. By default, this is the linear system dynamics x_k+1 = Ax_k +
    // Bu_k
    m_x = UpdateX(m_x, m_u, dt);

    // y = Cx + Du
    m_y = m_plant.CalculateY(m_x, m_u);

    // Add noise. If the user did not pass a noise vector to the
    // constructor, then this method will not do anything because
    // the standard deviations default to zero.
    m_y += frc::MakeWhiteNoiseVector<Outputs>(m_measurementStdDevs);
  }

  /**
   * Returns the current output of the plant.
   *
   * @return The current output of the plant.
   */
  const Eigen::Matrix<double, Outputs, 1>& GetOutput() const { return m_y; }

  /**
   * Returns an element of the current output of the plant.
   *
   * @param row The row to return.
   * @return An element of the current output of the plant.
   */
  double GetOutput(int row) const { return m_y(row); }

  /**
   * Sets the system inputs (usually voltages).
   *
   * @param u The system inputs.
   */
  void SetInput(const Eigen::Matrix<double, Inputs, 1>& u) {
    m_u = ClampInput(u);
  }

  /*
   * Sets the system inputs.
   *
   * @param row   The row in the input matrix to set.
   * @param value The value to set the row to.
   */
  void SetInput(int row, double value) {
    m_u(row, 0) = value;
    ClampInput(m_u);
  }

  /**
   * Sets the system state.
   *
   * @param state The new state.
   */
  void SetState(const Eigen::Matrix<double, States, 1>& state) { m_x = state; }

  /**
   * Returns the current drawn by this simulated system. Override this method to
   * add a custom current calculation.
   *
   * @return The current drawn by this simulated mechanism.
   */
  virtual units::ampere_t GetCurrentDraw() const {
    return units::ampere_t(0.0);
  }

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

  /**
   * Clamp the input vector such that no element exceeds the given voltage. If
   * any does, the relative magnitudes of the input will be maintained.
   *
   * @param u          The input vector.
   * @return The normalized input.
   */
  Eigen::Matrix<double, Inputs, 1> ClampInput(
      Eigen::Matrix<double, Inputs, 1> u) {
    return frc::NormalizeInputVector<Inputs>(
        u, frc::RobotController::GetInputVoltage());
  }

  LinearSystem<States, Inputs, Outputs> m_plant;

  Eigen::Matrix<double, States, 1> m_x;
  Eigen::Matrix<double, Outputs, 1> m_y;
  Eigen::Matrix<double, Inputs, 1> m_u;
  std::array<double, Outputs> m_measurementStdDevs;
};
}  // namespace frc::sim
