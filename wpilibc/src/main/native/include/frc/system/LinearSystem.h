/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>

#include <Eigen/Core>
#include <units/units.h>

#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"

namespace frc {

/**
 * A plant defined using state-space notation.
 *
 * A plant is a mathematical model of a system's dynamics.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
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
   * @param uMin Minimum control input.
   * @param uMax Maximum control input.
   */
  explicit LinearSystem(const Eigen::Matrix<double, States, States>& A,
                        const Eigen::Matrix<double, States, Inputs>& B,
                        const Eigen::Matrix<double, Outputs, States>& C,
                        const Eigen::Matrix<double, Outputs, Inputs>& D,
                        const Eigen::Matrix<double, Inputs, 1>& uMin,
                        const Eigen::Matrix<double, Inputs, 1>& uMax) {
    m_A = A;
    m_B = B;
    m_C = C;
    m_D = D;
    m_uMin = uMin;
    m_uMax = uMax;

    Reset();
  }

  virtual ~LinearSystem() = default;

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
   * Returns the minimum control input vector u.
   */
  const Eigen::Matrix<double, Inputs, 1>& Umin() const { return m_uMin; }

  /**
   * Returns an element of the minimum control input vector u.
   *
   * @param i Row of u.
   */
  double Umin(int i) const { return m_uMin(i, 0); }

  /**
   * Returns the maximum control input vector u.
   */
  const Eigen::Matrix<double, Inputs, 1>& Umax() const { return m_uMax; }

  /**
   * Returns an element of the maximum control input vector u.
   *
   * @param i Row of u.
   */
  double Umax(int i) const { return m_uMax(i, 0); }

  /**
   * Returns the current state x.
   */
  const Eigen::Matrix<double, States, 1>& X() const { return m_x; }

  /**
   * Returns an element of the current state x.
   *
   * @param i Row of x.
   */
  double X(int i) const { return m_x(i, 0); }

  /**
   * Returns the current measurement vector y.
   */
  const Eigen::Matrix<double, Outputs, 1>& Y() const { return m_y; }

  /**
   * Returns an element of the current measurement vector y.
   *
   * @param i Row of y.
   */
  double Y(int i) const { return m_y(i, 0); }

  /**
   * Returns the control input vector u.
   */
  const Eigen::Matrix<double, Inputs, 1>& U() const { return m_delayedU; }

  /**
   * Returns an element of the control input vector u.
   *
   * @param i Row of u.
   */
  double U(int i) const { return m_delayedU(i, 0); }

  /**
   * Set the initial state x.
   *
   * @param x The initial state.
   */
  void SetX(const Eigen::Matrix<double, States, 1>& x) { m_x = x; }

  /**
   * Set an element of the initial state x.
   *
   * @param i     Row of x.
   * @param value Value of element of x.
   */
  void SetX(int i, double value) { m_x(i, 0) = value; }

  /**
   * Set the current measurement y.
   *
   * @param y The current measurement.
   */
  void SetY(const Eigen::Matrix<double, Outputs, 1>& y) { m_y = y; }

  /**
   * Set an element of the current measurement y.
   *
   * @param i     Row of y.
   * @param value Value of element of y.
   */
  void SetY(int i, double value) { m_y(i, 0) = value; }

  /**
   * Resets the plant.
   */
  void Reset() {
    m_x.setZero();
    m_y.setZero();
    m_delayedU.setZero();
  }

  /**
   * Computes the new x and y given the control input.
   *
   * @param x The current state.
   * @param u The control input.
   * @param dt Timestep for model update.
   */
  void Update(const Eigen::Matrix<double, States, 1>& x,
              const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
    m_x = CalculateX(x, m_delayedU, dt);
    m_y = CalculateY(m_x, m_delayedU);
    m_delayedU = u;
  }

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
      const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) const {
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    DiscretizeAB<States, Inputs>(m_A, m_B, dt, &discA, &discB);

    return discA * x + discB * ClampInput(u);
  }

  /**
   * Computes the new y given the control input.
   *
   * This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x The current state.
   * @param u The control input.
   */
  Eigen::Matrix<double, Outputs, 1> CalculateY(
      const Eigen::Matrix<double, States, 1>& x,
      const Eigen::Matrix<double, Inputs, 1>& u) const {
    return m_C * x + m_D * ClampInput(u);
  }

  /**
   * Clamps input vector between system's minimum and maximum allowable input.
   *
   * @param u Input vector to clamp.
   * @return Clamped input vector.
   */
  Eigen::Matrix<double, Inputs, 1> ClampInput(
      const Eigen::Matrix<double, Inputs, 1>& u) const {
    Eigen::Matrix<double, Inputs, 1> result;
    for (int i = 0; i < Inputs; ++i) {
      result(i, 0) = std::clamp(u(i, 0), m_uMin(i, 0), m_uMax(i, 0));
    }
    return result;
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

  /**
   * Minimum allowable input vector.
   */
  Eigen::Matrix<double, Inputs, 1> m_uMin;

  /**
   * Maximum allowable input vector.
   */
  Eigen::Matrix<double, Inputs, 1> m_uMax;

  /**
   * State vector.
   */
  Eigen::Matrix<double, States, 1> m_x;

  /**
   * Output vector.
   */
  Eigen::Matrix<double, Outputs, 1> m_y;

  /**
   * Delayed u since predict and correct steps are run in reverse.
   */
  Eigen::Matrix<double, Inputs, 1> m_delayedU;
};

}  // namespace frc
