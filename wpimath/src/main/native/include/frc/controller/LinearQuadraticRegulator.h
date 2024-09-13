// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/EigenCore.h"
#include "frc/system/LinearSystem.h"
#include "units/time.h"

namespace frc {

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
class LinearQuadraticRegulator {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;

  using StateArray = wpi::array<double, States>;
  using InputArray = wpi::array<double, Inputs>;

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning
   * for how to select the tolerances.
   *
   * @tparam Outputs Number of outputs.
   * @param plant  The plant being controlled.
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   * @throws std::invalid_argument If the system is uncontrollable.
   */
  template <int Outputs>
  LinearQuadraticRegulator(const LinearSystem<States, Inputs, Outputs>& plant,
                           const StateArray& Qelems, const InputArray& Relems,
                           units::second_t dt);

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning
   * for how to select the tolerances.
   *
   * @param A      Continuous system matrix of the plant being controlled.
   * @param B      Continuous input matrix of the plant being controlled.
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   * @throws std::invalid_argument If the system is uncontrollable.
   */
  LinearQuadraticRegulator(const Matrixd<States, States>& A,
                           const Matrixd<States, Inputs>& B,
                           const StateArray& Qelems, const InputArray& Relems,
                           units::second_t dt);

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A  Continuous system matrix of the plant being controlled.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param Q  The state cost matrix.
   * @param R  The input cost matrix.
   * @param dt Discretization timestep.
   * @throws std::invalid_argument If the system is uncontrollable.
   */
  LinearQuadraticRegulator(const Matrixd<States, States>& A,
                           const Matrixd<States, Inputs>& B,
                           const Matrixd<States, States>& Q,
                           const Matrixd<Inputs, Inputs>& R,
                           units::second_t dt);

  /**
   * Constructs a controller with the given coefficients and plant.
   *
   * @param A  Continuous system matrix of the plant being controlled.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param Q  The state cost matrix.
   * @param R  The input cost matrix.
   * @param N  The state-input cross-term cost matrix.
   * @param dt Discretization timestep.
   * @throws std::invalid_argument If the system is uncontrollable.
   */
  LinearQuadraticRegulator(const Matrixd<States, States>& A,
                           const Matrixd<States, Inputs>& B,
                           const Matrixd<States, States>& Q,
                           const Matrixd<Inputs, Inputs>& R,
                           const Matrixd<States, Inputs>& N,
                           units::second_t dt);

  LinearQuadraticRegulator(LinearQuadraticRegulator&&) = default;
  LinearQuadraticRegulator& operator=(LinearQuadraticRegulator&&) = default;

  /**
   * Returns the controller matrix K.
   */
  const Matrixd<Inputs, States>& K() const { return m_K; }

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
  const StateVector& R() const { return m_r; }

  /**
   * Returns an element of the reference vector r.
   *
   * @param i Row of r.
   *
   * @return The row of the reference vector.
   */
  double R(int i) const { return m_r(i); }

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
  void Reset() {
    m_r.setZero();
    m_u.setZero();
  }

  /**
   * Returns the next output of the controller.
   *
   * @param x The current state x.
   */
  InputVector Calculate(const StateVector& x);

  /**
   * Returns the next output of the controller.
   *
   * @param x     The current state x.
   * @param nextR The next reference vector r.
   */
  InputVector Calculate(const StateVector& x, const StateVector& nextR);

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
                         units::second_t dt, units::second_t inputDelay);

 private:
  // Current reference
  StateVector m_r;

  // Computed controller output
  InputVector m_u;

  // Controller gain
  Matrixd<Inputs, States> m_K;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    LinearQuadraticRegulator<1, 1>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    LinearQuadraticRegulator<2, 1>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    LinearQuadraticRegulator<2, 2>;

}  // namespace frc

#include "LinearQuadraticRegulator.inc"
