// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <functional>

#include "Eigen/Core"
#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"
#include "units/time.h"

namespace frc {

/**
 * A plant defined using state-space notation.
 *
 * A plant is a mathematical model of a system's dynamics.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
template <int States, int Inputs, int Outputs>
class LinearSystem {
 public:
  /**
   * Constructs a discrete plant with the given continuous system coefficients.
   *
   * @param A    System matrix.
   * @param B    Input matrix.
   * @param C    Output matrix.
   * @param D    Feedthrough matrix.
   */
  LinearSystem(const Eigen::Matrix<double, States, States>& A,
               const Eigen::Matrix<double, States, Inputs>& B,
               const Eigen::Matrix<double, Outputs, States>& C,
               const Eigen::Matrix<double, Outputs, Inputs>& D) {
    m_A = A;
    m_B = B;
    m_C = C;
    m_D = D;
  }

  LinearSystem(const LinearSystem&) = default;
  LinearSystem& operator=(const LinearSystem&) = default;
  LinearSystem(LinearSystem&&) = default;
  LinearSystem& operator=(LinearSystem&&) = default;

  /**
   * Returns the system matrix A.
   */
  const Eigen::Matrix<double, States, States>& A() const { return m_A; }

  /**
   * Returns an element of the system matrix A.
   *
   * @param i Row of A.
   * @param j Column of A.
   */
  double A(int i, int j) const { return m_A(i, j); }

  /**
   * Returns the input matrix B.
   */
  const Eigen::Matrix<double, States, Inputs>& B() const { return m_B; }

  /**
   * Returns an element of the input matrix B.
   *
   * @param i Row of B.
   * @param j Column of B.
   */
  double B(int i, int j) const { return m_B(i, j); }

  /**
   * Returns the output matrix C.
   */
  const Eigen::Matrix<double, Outputs, States>& C() const { return m_C; }

  /**
   * Returns an element of the output matrix C.
   *
   * @param i Row of C.
   * @param j Column of C.
   */
  double C(int i, int j) const { return m_C(i, j); }

  /**
   * Returns the feedthrough matrix D.
   */
  const Eigen::Matrix<double, Outputs, Inputs>& D() const { return m_D; }

  /**
   * Returns an element of the feedthrough matrix D.
   *
   * @param i Row of D.
   * @param j Column of D.
   */
  double D(int i, int j) const { return m_D(i, j); }

  /**
   * Computes the new x given the old x and the control input.
   *
   * This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x  The current state.
   * @param u  The control input.
   * @param dt Timestep for model update.
   */
  Eigen::Matrix<double, States, 1> CalculateX(
      const Eigen::Matrix<double, States, 1>& x,
      const Eigen::Matrix<double, Inputs, 1>& clampedU,
      units::second_t dt) const {
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    DiscretizeAB<States, Inputs>(m_A, m_B, dt, &discA, &discB);

    return discA * x + discB * clampedU;
  }

  /**
   * Computes the new y given the control input.
   *
   * This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x The current state.
   * @param clampedU The control input.
   */
  Eigen::Matrix<double, Outputs, 1> CalculateY(
      const Eigen::Matrix<double, States, 1>& x,
      const Eigen::Matrix<double, Inputs, 1>& clampedU) const {
    return m_C * x + m_D * clampedU;
  }

 private:
  /**
   * Continuous system matrix.
   */
  Eigen::Matrix<double, States, States> m_A;

  /**
   * Continuous input matrix.
   */
  Eigen::Matrix<double, States, Inputs> m_B;

  /**
   * Output matrix.
   */
  Eigen::Matrix<double, Outputs, States> m_C;

  /**
   * Feedthrough matrix.
   */
  Eigen::Matrix<double, Outputs, Inputs> m_D;
};

}  // namespace frc
