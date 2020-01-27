/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <Eigen/Core>
#include <Eigen/QR>
#include <drake/math/discrete_algebraic_riccati_equation.h>
#include <units/units.h>

#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"
#include "frc/system/LinearSystem.h"

namespace frc {

/**
 * Contains the controller coefficients and logic for a state-space controller.
 *
 * State-space controllers generally use the control law u = -Kx. The
 * feedforward used is u_ff = K_ff * (r_k+1 - A * r_k).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
template <int States, int Inputs>
class LinearQuadraticRegulator {
 public:
  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param system The plant being controlled.
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   */
  template <int Outputs>
  LinearQuadraticRegulator(const LinearSystem<States, Inputs, Outputs>& plant,
                           const std::array<double, States>& Qelems,
                           const std::array<double, Inputs>& Relems,
                           units::second_t dt)
      : LinearQuadraticRegulator(plant.A(), plant.B(), Qelems, Relems, dt) {}

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A      Continuous system matrix of the plant being controlled.
   * @param B      Continuous input matrix of the plant being controlled.
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   */
  LinearQuadraticRegulator(const Eigen::Matrix<double, States, States>& A,
                           const Eigen::Matrix<double, States, Inputs>& B,
                           const std::array<double, States>& Qelems,
                           const std::array<double, Inputs>& Relems,
                           units::second_t dt)
      : m_A(A), m_B(B) {
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    DiscretizeAB(m_A, m_B, dt, &discA, &discB);

    Eigen::Matrix<double, States, States> Q = MakeCostMatrix(Qelems);
    Eigen::Matrix<double, Inputs, Inputs> R = MakeCostMatrix(Relems);

    auto S = drake::math::DiscreteAlgebraicRiccatiEquation(discA, discB, Q, R);
    Eigen::Matrix<double, Inputs, Inputs> tmp =
        discB.transpose() * S * discB + R;
    m_K = tmp.llt().solve(discB.transpose() * S * discA);
  }

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;

  /**
   * Enables controller.
   */
  void Enable() { m_enabled = true; }

  /**
   * Disables controller, zeroing controller output U.
   */
  void Disable() {
    m_enabled = false;
    m_u.setZero();
  }

  /**
   * Returns the controller matrix K.
   */
  const Eigen::Matrix<double, Inputs, States>& K() const { return m_K; }

  /**
   * Returns an element of the controller matrix K.
   *
   * @param i Row of K.
   * @param j Column of K.
   */
  double K(int i, int j) const { return m_K(i, j); }

  /**
   * Returns the reference vector r.
   */
  const Eigen::Matrix<double, States, 1>& R() const { return m_r; }

  /**
   * Returns an element of the reference vector r.
   *
   * @param i Row of r.
   */
  double R(int i) const { return m_r(i, 0); }

  /**
   * Returns the control input vector u.
   */
  const Eigen::Matrix<double, Inputs, 1>& U() const { return m_u; }

  /**
   * Returns an element of the control input vector u.
   *
   * @param i Row of u.
   */
  double U(int i) const { return m_u(i, 0); }

  /**
   * Resets the controller.
   */
  void Reset() {
    m_r.setZero();
    m_u.setZero();
  }

  /**
   * Update controller without setting a new reference.
   *
   * @param x The current state x.
   */
  void Update(const Eigen::Matrix<double, States, 1>& x) {
    if (m_enabled) {
      m_u = m_K * (m_r - x) + m_B.householderQr().solve(m_r - m_A * m_r);
    }
  }

  /**
   * Set a new reference and update the controller.
   *
   * @param x     The current state x.
   * @param nextR The next reference vector r.
   */
  void Update(const Eigen::Matrix<double, States, 1>& x,
              const Eigen::Matrix<double, States, 1>& nextR) {
    if (m_enabled) {
      m_u = m_K * (m_r - x) + m_B.householderQr().solve(nextR - m_A * m_r);
      m_r = nextR;
    }
  }

 private:
  Eigen::Matrix<double, States, States> m_A;
  Eigen::Matrix<double, States, Inputs> m_B;

  bool m_enabled = false;

  // Current reference.
  Eigen::Matrix<double, States, 1> m_r;

  // Computed controller output.
  Eigen::Matrix<double, Inputs, 1> m_u;

  // Controller gain.
  Eigen::Matrix<double, Inputs, States> m_K;
};

}  // namespace frc
