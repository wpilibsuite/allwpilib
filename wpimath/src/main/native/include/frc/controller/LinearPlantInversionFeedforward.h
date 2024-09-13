// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <functional>

#include <Eigen/QR>

#include "frc/EigenCore.h"
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
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 */
template <int States, int Inputs>
class LinearPlantInversionFeedforward {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;

  /**
   * Constructs a feedforward with the given plant.
   *
   * @tparam Outputs Number of outputs.
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
  LinearPlantInversionFeedforward(const Matrixd<States, States>& A,
                                  const Matrixd<States, Inputs>& B,
                                  units::second_t dt)
      : m_dt(dt) {
    DiscretizeAB<States, Inputs>(A, B, dt, &m_A, &m_B);
    Reset();
  }

  /**
   * Returns the previously calculated feedforward as an input vector.
   *
   * @return The calculated feedforward.
   */
  const InputVector& Uff() const { return m_uff; }

  /**
   * Returns an element of the previously calculated feedforward.
   *
   * @param i Row of uff.
   *
   * @return The row of the calculated feedforward.
   */
  double Uff(int i) const { return m_uff(i); }

  /**
   * Returns the current reference vector r.
   *
   * @return The current reference vector.
   */
  const StateVector& R() const { return m_r; }

  /**
   * Returns an element of the reference vector r.
   *
   * @param i Row of r.
   *
   * @return The row of the current reference vector.
   */
  double R(int i) const { return m_r(i); }

  /**
   * Resets the feedforward with a specified initial state vector.
   *
   * @param initialState The initial state vector.
   */
  void Reset(const StateVector& initialState) {
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
   * Reset(const StateVector&). If the initial state is not
   * set it defaults to a zero vector.
   *
   * @param nextR The reference state of the future timestep (k + dt).
   *
   * @return The calculated feedforward.
   */
  InputVector Calculate(const StateVector& nextR) {
    return Calculate(m_r, nextR);
  }

  /**
   * Calculate the feedforward with current and future reference vectors.
   *
   * @param r     The reference state of the current timestep (k).
   * @param nextR The reference state of the future timestep (k + dt).
   *
   * @return The calculated feedforward.
   */
  InputVector Calculate(const StateVector& r, const StateVector& nextR) {
    // rₖ₊₁ = Arₖ + Buₖ
    // Buₖ = rₖ₊₁ − Arₖ
    // uₖ = B⁺(rₖ₊₁ − Arₖ)
    m_uff = m_B.householderQr().solve(nextR - (m_A * r));
    m_r = nextR;
    return m_uff;
  }

 private:
  Matrixd<States, States> m_A;
  Matrixd<States, Inputs> m_B;

  units::second_t m_dt;

  // Current reference
  StateVector m_r;

  // Computed feedforward
  InputVector m_uff;
};

}  // namespace frc
