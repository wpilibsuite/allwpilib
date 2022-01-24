// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/fmt/Eigen.h>

#include <string>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "Eigen/Cholesky"
#include "Eigen/Core"
#include "Eigen/Eigenvalues"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"
#include "frc/system/LinearSystem.h"
#include "units/time.h"
#include "unsupported/Eigen/MatrixFunctions"
#include "wpimath/MathShared.h"

namespace frc {
namespace detail {

/**
 * Contains the controller coefficients and logic for a linear-quadratic
 * regulator (LQR).
 * LQRs use the control law u = K(r - x).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
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
   * @param A  Continuous system matrix of the plant being controlled.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param Q  The state cost matrix.
   * @param R  The input cost matrix.
   * @param dt Discretization timestep.
   */
  LinearQuadraticRegulatorImpl(const Eigen::Matrix<double, States, States>& A,
                               const Eigen::Matrix<double, States, Inputs>& B,
                               const Eigen::Matrix<double, States, States>& Q,
                               const Eigen::Matrix<double, Inputs, Inputs>& R,
                               units::second_t dt) {
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    DiscretizeAB<States, Inputs>(A, B, dt, &discA, &discB);

    if (!IsStabilizable<States, Inputs>(discA, discB)) {
      std::string msg = fmt::format(
          "The system passed to the LQR is uncontrollable!\n\nA =\n{}\nB "
          "=\n{}\n",
          discA, discB);

      wpi::math::MathSharedStore::ReportError(msg);
      throw std::invalid_argument(msg);
    }

    Eigen::Matrix<double, States, States> S =
        drake::math::DiscreteAlgebraicRiccatiEquation(discA, discB, Q, R);

    // K = (BᵀSB + R)⁻¹BᵀSA
    m_K = (discB.transpose() * S * discB + R)
              .llt()
              .solve(discB.transpose() * S * discA);

    Reset();
  }

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A  Continuous system matrix of the plant being controlled.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param Q  The state cost matrix.
   * @param R  The input cost matrix.
   * @param N  The state-input cross-term cost matrix.
   * @param dt Discretization timestep.
   */
  LinearQuadraticRegulatorImpl(const Eigen::Matrix<double, States, States>& A,
                               const Eigen::Matrix<double, States, Inputs>& B,
                               const Eigen::Matrix<double, States, States>& Q,
                               const Eigen::Matrix<double, Inputs, Inputs>& R,
                               const Eigen::Matrix<double, States, Inputs>& N,
                               units::second_t dt) {
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    DiscretizeAB<States, Inputs>(A, B, dt, &discA, &discB);

    Eigen::Matrix<double, States, States> S =
        drake::math::DiscreteAlgebraicRiccatiEquation(discA, discB, Q, R, N);

    // K = (BᵀSB + R)⁻¹(BᵀSA + Nᵀ)
    m_K = (discB.transpose() * S * discB + R)
              .llt()
              .solve(discB.transpose() * S * discA + N.transpose());

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
  const Eigen::Vector<double, States>& R() const { return m_r; }

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
  const Eigen::Vector<double, Inputs>& U() const { return m_u; }

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
  Eigen::Vector<double, Inputs> Calculate(
      const Eigen::Vector<double, States>& x) {
    m_u = m_K * (m_r - x);
    return m_u;
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x     The current state x.
   * @param nextR The next reference vector r.
   */
  Eigen::Vector<double, Inputs> Calculate(
      const Eigen::Vector<double, States>& x,
      const Eigen::Vector<double, States>& nextR) {
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
  Eigen::Vector<double, States> m_r;

  // Computed controller output
  Eigen::Vector<double, Inputs> m_u;

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
   * @tparam Outputs The number of outputs.
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
   * @param A  Continuous system matrix of the plant being controlled.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param Q  The state cost matrix.
   * @param R  The input cost matrix.
   * @param dt Discretization timestep.
   */
  LinearQuadraticRegulator(const Eigen::Matrix<double, States, States>& A,
                           const Eigen::Matrix<double, States, Inputs>& B,
                           const Eigen::Matrix<double, States, States>& Q,
                           const Eigen::Matrix<double, Inputs, Inputs>& R,
                           units::second_t dt)
      : detail::LinearQuadraticRegulatorImpl<States, Inputs>{A, B, Q, R, dt} {}

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A  Continuous system matrix of the plant being controlled.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param Q  The state cost matrix.
   * @param R  The input cost matrix.
   * @param N  The state-input cross-term cost matrix.
   * @param dt Discretization timestep.
   */
  LinearQuadraticRegulator(const Eigen::Matrix<double, States, States>& A,
                           const Eigen::Matrix<double, States, Inputs>& B,
                           const Eigen::Matrix<double, States, States>& Q,
                           const Eigen::Matrix<double, Inputs, Inputs>& R,
                           const Eigen::Matrix<double, States, Inputs>& N,
                           units::second_t dt)
      : detail::LinearQuadraticRegulatorImpl<States, Inputs>{A, B, Q,
                                                             R, N, dt} {}

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;
};

// Template specializations are used here to make common state-input pairs
// compile faster.
template <>
class WPILIB_DLLEXPORT LinearQuadraticRegulator<1, 1>
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

  LinearQuadraticRegulator(const Eigen::Matrix<double, 1, 1>& A,
                           const Eigen::Matrix<double, 1, 1>& B,
                           const Eigen::Matrix<double, 1, 1>& Q,
                           const Eigen::Matrix<double, 1, 1>& R,
                           const Eigen::Matrix<double, 1, 1>& N,
                           units::second_t dt);

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;
};

// Template specializations are used here to make common state-input pairs
// compile faster.
template <>
class WPILIB_DLLEXPORT LinearQuadraticRegulator<2, 1>
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

  LinearQuadraticRegulator(const Eigen::Matrix<double, 2, 2>& A,
                           const Eigen::Matrix<double, 2, 1>& B,
                           const Eigen::Matrix<double, 2, 2>& Q,
                           const Eigen::Matrix<double, 1, 1>& R,
                           const Eigen::Matrix<double, 2, 1>& N,
                           units::second_t dt);

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;
};

// Template specializations are used here to make common state-input pairs
// compile faster.
template <>
class WPILIB_DLLEXPORT LinearQuadraticRegulator<2, 2>
    : public detail::LinearQuadraticRegulatorImpl<2, 2> {
 public:
  template <int Outputs>
  LinearQuadraticRegulator(const LinearSystem<2, 2, Outputs>& plant,
                           const wpi::array<double, 2>& Qelems,
                           const wpi::array<double, 2>& Relems,
                           units::second_t dt)
      : LinearQuadraticRegulator(plant.A(), plant.B(), Qelems, Relems, dt) {}

  LinearQuadraticRegulator(const Eigen::Matrix<double, 2, 2>& A,
                           const Eigen::Matrix<double, 2, 2>& B,
                           const wpi::array<double, 2>& Qelems,
                           const wpi::array<double, 2>& Relems,
                           units::second_t dt);

  LinearQuadraticRegulator(const Eigen::Matrix<double, 2, 2>& A,
                           const Eigen::Matrix<double, 2, 2>& B,
                           const Eigen::Matrix<double, 2, 2>& Q,
                           const Eigen::Matrix<double, 2, 2>& R,
                           units::second_t dt);

  LinearQuadraticRegulator(const Eigen::Matrix<double, 2, 2>& A,
                           const Eigen::Matrix<double, 2, 2>& B,
                           const Eigen::Matrix<double, 2, 2>& Q,
                           const Eigen::Matrix<double, 2, 2>& R,
                           const Eigen::Matrix<double, 2, 2>& N,
                           units::second_t dt);

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;
};

}  // namespace frc
