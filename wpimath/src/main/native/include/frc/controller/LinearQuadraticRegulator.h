// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/array.h>

#include "Eigen/Core"
#include "Eigen/src/Cholesky/LLT.h"
#include "Eigen/src/Eigenvalues/ComplexSchur.h"
#include "Eigen/src/LU/Determinant.h"
#include "Eigen/src/LU/InverseImpl.h"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"
#include "frc/system/LinearSystem.h"
#include "units/time.h"
#include "unsupported/Eigen/src/MatrixFunctions/MatrixPower.h"
#include "unsupported/Eigen/src/MatrixFunctions/MatrixSquareRoot.h"

namespace frc {
namespace detail {

/**
 * Contains the controller coefficients and logic for a linear-quadratic
 * regulator (LQR).
 * LQRs use the control law u = K(r - x).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
template <int States, int Inputs>
class LinearQuadraticRegulatorImpl {
 public:
  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param plant  The plant being controlled.
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   */
  template <int Outputs>
  LinearQuadraticRegulatorImpl(
      const LinearSystem<States, Inputs, Outputs>& plant,
      const wpi::array<double, States>& Qelems,
      const wpi::array<double, Inputs>& Relems, units::second_t dt)
      : LinearQuadraticRegulatorImpl(plant.A(), plant.B(), Qelems, Relems, dt) {
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A      Continuous system matrix of the plant being controlled.
   * @param B      Continuous input matrix of the plant being controlled.
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   */
  LinearQuadraticRegulatorImpl(const Eigen::Matrix<double, States, States>& A,
                               const Eigen::Matrix<double, States, Inputs>& B,
                               const wpi::array<double, States>& Qelems,
                               const wpi::array<double, Inputs>& Relems,
                               units::second_t dt)
      : LinearQuadraticRegulatorImpl(A, B, MakeCostMatrix(Qelems),
                                     MakeCostMatrix(Relems), dt) {}

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A      Continuous system matrix of the plant being controlled.
   * @param B      Continuous input matrix of the plant being controlled.
   * @param Q      The state cost matrix.
   * @param R      The input cost matrix.
   * @param dt     Discretization timestep.
   */
  LinearQuadraticRegulatorImpl(const Eigen::Matrix<double, States, States>& A,
                               const Eigen::Matrix<double, States, Inputs>& B,
                               const Eigen::Matrix<double, States, States>& Q,
                               const Eigen::Matrix<double, Inputs, Inputs>& R,
                               units::second_t dt) {
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    DiscretizeAB<States, Inputs>(A, B, dt, &discA, &discB);

    Eigen::Matrix<double, States, States> S =
        drake::math::DiscreteAlgebraicRiccatiEquation(discA, discB, Q, R);
    Eigen::Matrix<double, Inputs, Inputs> tmp =
        discB.transpose() * S * discB + R;
    m_K = tmp.llt().solve(discB.transpose() * S * discA);

    Reset();
  }

  LinearQuadraticRegulatorImpl(LinearQuadraticRegulatorImpl&&) = default;
  LinearQuadraticRegulatorImpl& operator=(LinearQuadraticRegulatorImpl&&) =
      default;

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
   *
   * @return The reference vector.
   */
  const Eigen::Matrix<double, States, 1>& R() const { return m_r; }

  /**
   * Returns an element of the reference vector r.
   *
   * @param i Row of r.
   *
   * @return The row of the reference vector.
   */
  double R(int i) const { return m_r(i, 0); }

  /**
   * Returns the control input vector u.
   *
   * @return The control input.
   */
  const Eigen::Matrix<double, Inputs, 1>& U() const { return m_u; }

  /**
   * Returns an element of the control input vector u.
   *
   * @param i Row of u.
   *
   * @return The row of the control input vector.
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
   * Returns the next output of the controller.
   *
   * @param x The current state x.
   */
  Eigen::Matrix<double, Inputs, 1> Calculate(
      const Eigen::Matrix<double, States, 1>& x) {
    m_u = m_K * (m_r - x);
    return m_u;
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x     The current state x.
   * @param nextR The next reference vector r.
   */
  Eigen::Matrix<double, Inputs, 1> Calculate(
      const Eigen::Matrix<double, States, 1>& x,
      const Eigen::Matrix<double, States, 1>& nextR) {
    m_r = nextR;
    return Calculate(x);
  }

  /**
   * Adjusts LQR controller gain to compensate for a pure time delay in the
   * input.
   *
   * Linear-Quadratic regulator controller gains tend to be aggressive. If
   * sensor measurements are time-delayed too long, the LQR may be unstable.
   * However, if we know the amount of delay, we can compute the control based
   * on where the system will be after the time delay.
   *
   * See https://file.tavsys.net/control/controls-engineering-in-frc.pdf
   * appendix C.4 for a derivation.
   *
   * @param plant      The plant being controlled.
   * @param dt         Discretization timestep.
   * @param inputDelay Input time delay.
   */
  template <int Outputs>
  void LatencyCompensate(const LinearSystem<States, Inputs, Outputs>& plant,
                         units::second_t dt, units::second_t inputDelay) {
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    DiscretizeAB<States, Inputs>(plant.A(), plant.B(), dt, &discA, &discB);
    m_K = m_K * (discA - discB * m_K).pow(inputDelay / dt);
  }

 private:
  // Current reference
  Eigen::Matrix<double, States, 1> m_r;

  // Computed controller output
  Eigen::Matrix<double, Inputs, 1> m_u;

  // Controller gain
  Eigen::Matrix<double, Inputs, States> m_K;
};

}  // namespace detail

template <int States, int Inputs>
class LinearQuadraticRegulator
    : public detail::LinearQuadraticRegulatorImpl<States, Inputs> {
 public:
  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param plant  The plant being controlled.
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   */
  template <int Outputs>
  LinearQuadraticRegulator(const LinearSystem<States, Inputs, Outputs>& plant,
                           const wpi::array<double, States>& Qelems,
                           const wpi::array<double, Inputs>& Relems,
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
                           const wpi::array<double, States>& Qelems,
                           const wpi::array<double, Inputs>& Relems,
                           units::second_t dt)
      : LinearQuadraticRegulator(A, B, MakeCostMatrix(Qelems),
                                 MakeCostMatrix(Relems), dt) {}

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A      Continuous system matrix of the plant being controlled.
   * @param B      Continuous input matrix of the plant being controlled.
   * @param Q      The state cost matrix.
   * @param R      The input cost matrix.
   * @param dt     Discretization timestep.
   */
  LinearQuadraticRegulator(const Eigen::Matrix<double, States, States>& A,
                           const Eigen::Matrix<double, States, Inputs>& B,
                           const Eigen::Matrix<double, States, States>& Q,
                           const Eigen::Matrix<double, Inputs, Inputs>& R,
                           units::second_t dt)
      : detail::LinearQuadraticRegulatorImpl<States, Inputs>{A, B, Q, R, dt} {}

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;
};

// Template specializations are used here to make common state-input pairs
// compile faster.
template <>
class LinearQuadraticRegulator<1, 1>
    : public detail::LinearQuadraticRegulatorImpl<1, 1> {
 public:
  template <int Outputs>
  LinearQuadraticRegulator(const LinearSystem<1, 1, Outputs>& plant,
                           const wpi::array<double, 1>& Qelems,
                           const wpi::array<double, 1>& Relems,
                           units::second_t dt)
      : LinearQuadraticRegulator(plant.A(), plant.B(), Qelems, Relems, dt) {}

  LinearQuadraticRegulator(const Eigen::Matrix<double, 1, 1>& A,
                           const Eigen::Matrix<double, 1, 1>& B,
                           const wpi::array<double, 1>& Qelems,
                           const wpi::array<double, 1>& Relems,
                           units::second_t dt);

  LinearQuadraticRegulator(const Eigen::Matrix<double, 1, 1>& A,
                           const Eigen::Matrix<double, 1, 1>& B,
                           const Eigen::Matrix<double, 1, 1>& Q,
                           const Eigen::Matrix<double, 1, 1>& R,
                           units::second_t dt);

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;
};

// Template specializations are used here to make common state-input pairs
// compile faster.
template <>
class LinearQuadraticRegulator<2, 1>
    : public detail::LinearQuadraticRegulatorImpl<2, 1> {
 public:
  template <int Outputs>
  LinearQuadraticRegulator(const LinearSystem<2, 1, Outputs>& plant,
                           const wpi::array<double, 2>& Qelems,
                           const wpi::array<double, 1>& Relems,
                           units::second_t dt)
      : LinearQuadraticRegulator(plant.A(), plant.B(), Qelems, Relems, dt) {}

  LinearQuadraticRegulator(const Eigen::Matrix<double, 2, 2>& A,
                           const Eigen::Matrix<double, 2, 1>& B,
                           const wpi::array<double, 2>& Qelems,
                           const wpi::array<double, 1>& Relems,
                           units::second_t dt);

  LinearQuadraticRegulator(const Eigen::Matrix<double, 2, 2>& A,
                           const Eigen::Matrix<double, 2, 1>& B,
                           const Eigen::Matrix<double, 2, 2>& Q,
                           const Eigen::Matrix<double, 1, 1>& R,
                           units::second_t dt);

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;
};

}  // namespace frc
