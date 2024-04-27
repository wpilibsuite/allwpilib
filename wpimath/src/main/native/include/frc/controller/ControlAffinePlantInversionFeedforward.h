// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <array>
#include <functional>

#include <Eigen/QR>

#include "frc/EigenCore.h"
#include "frc/system/NumericalJacobian.h"
#include "units/time.h"

namespace frc {

/**
 * Constructs a control-affine plant inversion model-based feedforward from
 * given model dynamics.
 *
 * If given the vector valued function as f(x, u) where x is the state
 * vector and u is the input vector, the B matrix(continuous input matrix)
 * is calculated through a NumericalJacobian. In this case f has to be
 * control-affine (of the form f(x) + Bu).
 *
 * The feedforward is calculated as
 * <strong> u_ff = B<sup>+</sup> (rDot - f(x)) </strong>, where <strong>
 * B<sup>+</sup> </strong> is the pseudoinverse of B.
 *
 * This feedforward does not account for a dynamic B matrix, B is either
 * determined or supplied when the feedforward is created and remains constant.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 */
template <int States, int Inputs>
class ControlAffinePlantInversionFeedforward {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;

  /**
   * Constructs a feedforward with given model dynamics as a function
   * of state and input.
   *
   * @param f  A vector-valued function of x, the state, and
   *           u, the input, that returns the derivative of
   *           the state vector. HAS to be control-affine
   *           (of the form f(x) + Bu).
   * @param dt The timestep between calls of calculate().
   */
  ControlAffinePlantInversionFeedforward(
      std::function<StateVector(const StateVector&, const InputVector&)> f,
      units::second_t dt)
      : m_dt(dt), m_f(f) {
    m_B = NumericalJacobianU<States, States, Inputs>(f, StateVector::Zero(),
                                                     InputVector::Zero());

    Reset();
  }

  /**
   * Constructs a feedforward with given model dynamics as a function of state,
   * and the plant's B matrix(continuous input matrix).
   *
   * @param f  A vector-valued function of x, the state,
   *           that returns the derivative of the state vector.
   * @param B  Continuous input matrix of the plant being controlled.
   * @param dt The timestep between calls of calculate().
   */
  ControlAffinePlantInversionFeedforward(
      std::function<StateVector(const StateVector&)> f,
      const Matrixd<States, Inputs>& B, units::second_t dt)
      : m_B(B), m_dt(dt) {
    m_f = [=](const StateVector& x, const InputVector& u) -> StateVector {
      return f(x);
    };

    Reset();
  }

  ControlAffinePlantInversionFeedforward(
      ControlAffinePlantInversionFeedforward&&) = default;
  ControlAffinePlantInversionFeedforward& operator=(
      ControlAffinePlantInversionFeedforward&&) = default;

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
    StateVector rDot = (nextR - r) / m_dt.value();

    // ṙ = f(r) + Bu
    // Bu = ṙ − f(r)
    // u = B⁺(ṙ − f(r))
    m_uff = m_B.householderQr().solve(rDot - m_f(r, InputVector::Zero()));

    m_r = nextR;
    return m_uff;
  }

 private:
  Matrixd<States, Inputs> m_B;

  units::second_t m_dt;

  /**
   * The model dynamics.
   */
  std::function<StateVector(const StateVector&, const InputVector&)> m_f;

  // Current reference
  StateVector m_r;

  // Computed feedforward
  InputVector m_uff;
};

}  // namespace frc
