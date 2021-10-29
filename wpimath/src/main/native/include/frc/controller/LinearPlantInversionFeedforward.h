// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <functional>

#include "Eigen/Core"
#include "Eigen/QR"
#include "frc/system/Discretization.h"
#include "frc/system/LinearSystem.h"
#include "units/time.h"

namespace frc {

/**
 * Constructs a plant inversion model-based feedforward from a LinearSystem.
 *
 * The feedforward is calculated as <strong> u_ff = B<sup>+</sup> (r_k+1 - A
 * r_k) </strong>, where <strong> B<sup>+</sup> </strong> is the pseudoinverse
 * of B.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 *
 * @tparam States The number of states.
 * @tparam Inputs The number of inputs.
 */
template <int States, int Inputs>
class LinearPlantInversionFeedforward {
 public:
  /**
   * Constructs a feedforward with the given plant.
   *
   * @tparam Outputs The number of outputs.
   * @param plant The plant being controlled.
   * @param dt    Discretization timestep.
   */
  template <int Outputs>
  LinearPlantInversionFeedforward(
      const LinearSystem<States, Inputs, Outputs>& plant, units::second_t dt)
      : LinearPlantInversionFeedforward(plant.A(), plant.B(), dt) {}

  /**
   * Constructs a feedforward with the given coefficients.
   *
   * @param A  Continuous system matrix of the plant being controlled.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param dt Discretization timestep.
   */
  LinearPlantInversionFeedforward(
      const Eigen::Matrix<double, States, States>& A,
      const Eigen::Matrix<double, States, Inputs>& B, units::second_t dt)
      : m_dt(dt) {
    DiscretizeAB<States, Inputs>(A, B, dt, &m_A, &m_B);
    Reset();
  }

  /**
   * Returns the previously calculated feedforward as an input vector.
   *
   * @return The calculated feedforward.
   */
  const Eigen::Vector<double, Inputs>& Uff() const { return m_uff; }

  /**
   * Returns an element of the previously calculated feedforward.
   *
   * @param i Row of uff.
   *
   * @return The row of the calculated feedforward.
   */
  double Uff(int i) const { return m_uff(i, 0); }

  /**
   * Returns the current reference vector r.
   *
   * @return The current reference vector.
   */
  const Eigen::Vector<double, States>& R() const { return m_r; }

  /**
   * Returns an element of the reference vector r.
   *
   * @param i Row of r.
   *
   * @return The row of the current reference vector.
   */
  double R(int i) const { return m_r(i, 0); }

  /**
   * Resets the feedforward with a specified initial state vector.
   *
   * @param initialState The initial state vector.
   */
  void Reset(const Eigen::Vector<double, States>& initialState) {
    m_r = initialState;
    m_uff.setZero();
  }

  /**
   * Resets the feedforward with a zero initial state vector.
   */
  void Reset() {
    m_r.setZero();
    m_uff.setZero();
  }

  /**
   * Calculate the feedforward with only the desired
   * future reference. This uses the internally stored "current"
   * reference.
   *
   * If this method is used the initial state of the system is the one set using
   * Reset(const Eigen::Vector<double, States>&). If the initial state is not
   * set it defaults to a zero vector.
   *
   * @param nextR The reference state of the future timestep (k + dt).
   *
   * @return The calculated feedforward.
   */
  Eigen::Vector<double, Inputs> Calculate(
      const Eigen::Vector<double, States>& nextR) {
    return Calculate(m_r, nextR);  // NOLINT
  }

  /**
   * Calculate the feedforward with current and future reference vectors.
   *
   * @param r     The reference state of the current timestep (k).
   * @param nextR The reference state of the future timestep (k + dt).
   *
   * @return The calculated feedforward.
   */
  Eigen::Vector<double, Inputs> Calculate(
      const Eigen::Vector<double, States>& r,
      const Eigen::Vector<double, States>& nextR) {
    m_uff = m_B.householderQr().solve(nextR - (m_A * r));
    m_r = nextR;
    return m_uff;
  }

 private:
  Eigen::Matrix<double, States, States> m_A;
  Eigen::Matrix<double, States, Inputs> m_B;

  units::second_t m_dt;

  // Current reference
  Eigen::Vector<double, States> m_r;

  // Computed feedforward
  Eigen::Vector<double, Inputs> m_uff;
};

}  // namespace frc
