// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/system/Discretization.h>
#include <frc/system/LinearSystem.h>

#include "Eigen/Core"
#include "Eigen/QR"
#include "units/time.h"

namespace frc {

/**
 * Contains the controller coefficients and logic for an implicit model
 * follower.
 *
 * Implicit model following lets us design a feedback controller that erases the
 * dynamics of our system and makes it behave like some other system. This can
 * be used to make a drivetrain more controllable during teleop driving by
 * making it behave like a slower or more benign drivetrain.
 *
 * For more on the underlying math, read appendix B.3 in
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
template <int States, int Inputs>
class ImplicitModelFollower {
 public:
  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param plant    The plant being controlled.
   * @param plantRef The plant whose dynamics should be followed.
   * @param dt       Discretization timestep.
   */
  template <int Outputs>
  ImplicitModelFollower(const LinearSystem<States, Inputs, Outputs>& plant,
                        const LinearSystem<States, Inputs, Outputs>& plantRef,
                        units::second_t dt)
      : ImplicitModelFollower<States, Inputs>(plant.A(), plant.B(),
                                              plantRef.A(), plantRef.B(), dt) {}

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A      Continuous system matrix of the plant being controlled.
   * @param B      Continuous input matrix of the plant being controlled.
   * @param Aref   Continuous system matrix whose dynamics should be followed.
   * @param Bref   Continuous input matrix whose dynamics should be followed.
   * @param dt     Discretization timestep.
   */
  ImplicitModelFollower(const Eigen::Matrix<double, States, States>& A,
                        const Eigen::Matrix<double, States, Inputs>& B,
                        const Eigen::Matrix<double, States, States>& Aref,
                        const Eigen::Matrix<double, States, Inputs>& Bref,
                        units::second_t dt) {
    // Discretize real dynamics
    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, Inputs> discB;
    frc::DiscretizeAB<States, Inputs>(A, B, dt, &discA, &discB);

    // Discretize desired dynamics
    Eigen::Matrix<double, States, States> discAref;
    Eigen::Matrix<double, States, Inputs> discBref;
    frc::DiscretizeAB<States, Inputs>(Aref, Bref, dt, &discAref, &discBref);

    // Find u_imf that makes real model match reference model.
    //
    // x_k+1 = Ax_k + Bu_imf
    // z_k+1 = Aref z_k + Bref u_k
    //
    // Let x_k = z_k.
    //
    // x_k+1 = z_k+1
    // Ax_k + Bu_imf = Aref x_k + Bref u_k
    // Bu_imf = Aref x_k - Ax_k + Bref u_k
    // Bu_imf = (Aref - A)x_k + Bref u_k
    // u_imf = B^+ ((Aref - A)x_k + Bref u_k)
    // u_imf = -B^+ (A - Aref)x_k + B^+ Bref u_k

    // The first term makes the open-loop poles that of the reference
    // system, and the second term makes the input behave like that of the
    // reference system.
    m_A = -discB.householderQr().solve(discA - discAref);
    m_B = discB.householderQr().solve(discBref);

    Reset();
  }

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
  double U(int i) const { return m_u(i); }

  /**
   * Resets the controller.
   */
  void Reset() { m_u.setZero(); }

  /**
   * Returns the next output of the controller.
   *
   * @param x The current state x.
   * @param u The current input for the original model.
   */
  Eigen::Vector<double, Inputs> Calculate(
      const Eigen::Vector<double, States>& x,
      const Eigen::Vector<double, Inputs>& u) {
    m_u = m_A * x + m_B * u;
    return m_u;
  }

 private:
  // Computed controller output
  Eigen::Vector<double, Inputs> m_u;

  // State space conversion gain
  Eigen::Matrix<double, Inputs, States> m_A;

  // Input space conversion gain
  Eigen::Matrix<double, Inputs, Inputs> m_B;
};

}  // namespace frc
