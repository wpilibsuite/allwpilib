// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/system/LinearSystem.h>

#include <Eigen/QR>

#include "frc/EigenCore.h"
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
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 */
template <int States, int Inputs>
class ImplicitModelFollower {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param plant    The plant being controlled.
   * @param plantRef The plant whose dynamics should be followed.
   */
  template <int Outputs>
  ImplicitModelFollower(const LinearSystem<States, Inputs, Outputs>& plant,
                        const LinearSystem<States, Inputs, Outputs>& plantRef)
      : ImplicitModelFollower<States, Inputs>(plant.A(), plant.B(),
                                              plantRef.A(), plantRef.B()) {}

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A    Continuous system matrix of the plant being controlled.
   * @param B    Continuous input matrix of the plant being controlled.
   * @param Aref Continuous system matrix whose dynamics should be followed.
   * @param Bref Continuous input matrix whose dynamics should be followed.
   */
  ImplicitModelFollower(const Matrixd<States, States>& A,
                        const Matrixd<States, Inputs>& B,
                        const Matrixd<States, States>& Aref,
                        const Matrixd<States, Inputs>& Bref) {
    // Find u_imf that makes real model match reference model.
    //
    // dx/dt = Ax + Bu_imf
    // dz/dt = A_ref z + B_ref u
    //
    // Let x = z.
    //
    // dx/dt = dz/dt
    // Ax + Bu_imf = Aref x + B_ref u
    // Bu_imf = A_ref x - Ax + B_ref u
    // Bu_imf = (A_ref - A)x + B_ref u
    // u_imf = B⁻¹((A_ref - A)x + Bref u)
    // u_imf = -B⁻¹(A - A_ref)x + B⁻¹B_ref u

    // The first term makes the open-loop poles that of the reference
    // system, and the second term makes the input behave like that of the
    // reference system.
    m_A = -B.householderQr().solve(A - Aref);
    m_B = B.householderQr().solve(Bref);

    Reset();
  }

  /**
   * Returns the control input vector u.
   *
   * @return The control input.
   */
  const InputVector& U() const { return m_u; }

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
  InputVector Calculate(const StateVector& x, const InputVector& u) {
    m_u = m_A * x + m_B * u;
    return m_u;
  }

 private:
  // Computed controller output
  InputVector m_u;

  // State space conversion gain
  Matrixd<Inputs, States> m_A;

  // Input space conversion gain
  Matrixd<Inputs, Inputs> m_B;
};

}  // namespace frc
