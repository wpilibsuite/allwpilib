// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/array.h>

#include "frc/EigenCore.h"
#include "units/time.h"

namespace frc {

/**
 * A Kalman filter combines predictions from a model and measurements to give an
 * estimate of the true system state. This is useful because many states cannot
 * be measured directly as a result of sensor noise, or because the state is
 * "hidden".
 *
 * Kalman filters use a K gain matrix to determine whether to trust the model or
 * measurements more. Kalman filter theory uses statistics to compute an optimal
 * K gain which minimizes the sum of squares error in the state estimate. This K
 * gain is used to correct the state estimate by some amount of the difference
 * between the actual measurements and the measurements predicted by the model.
 *
 * An extended Kalman filter supports nonlinear state and measurement models. It
 * propagates the error covariance by linearizing the models around the state
 * estimate, then applying the linear Kalman filter equations.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9
 * "Stochastic control theory".
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @tparam Outputs Number of outputs.
 */
template <int States, int Inputs, int Outputs>
class ExtendedKalmanFilter {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;
  using OutputVector = Vectord<Outputs>;

  using StateArray = wpi::array<double, States>;
  using OutputArray = wpi::array<double, Outputs>;

  using StateMatrix = Matrixd<States, States>;

  /**
   * Constructs an extended Kalman filter.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices
   * for how to select the standard deviations.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  ExtendedKalmanFilter(
      std::function<StateVector(const StateVector&, const InputVector&)> f,
      std::function<OutputVector(const StateVector&, const InputVector&)> h,
      const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
      units::second_t dt);

  /**
   * Constructs an extended Kalman filter.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices
   * for how to select the standard deviations.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param residualFuncY      A function that computes the residual of two
   *                           measurement vectors (i.e. it subtracts them.)
   * @param addFuncX           A function that adds two state vectors.
   * @param dt                 Nominal discretization timestep.
   */
  ExtendedKalmanFilter(
      std::function<StateVector(const StateVector&, const InputVector&)> f,
      std::function<OutputVector(const StateVector&, const InputVector&)> h,
      const StateArray& stateStdDevs, const OutputArray& measurementStdDevs,
      std::function<OutputVector(const OutputVector&, const OutputVector&)>
          residualFuncY,
      std::function<StateVector(const StateVector&, const StateVector&)>
          addFuncX,
      units::second_t dt);

  /**
   * Returns the error covariance matrix P.
   */
  const StateMatrix& P() const { return m_P; }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param i Row of P.
   * @param j Column of P.
   */
  double P(int i, int j) const { return m_P(i, j); }

  /**
   * Set the current error covariance matrix P.
   *
   * @param P The error covariance matrix P.
   */
  void SetP(const StateMatrix& P) { m_P = P; }

  /**
   * Returns the state estimate x-hat.
   */
  const StateVector& Xhat() const { return m_xHat; }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const { return m_xHat(i); }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  void SetXhat(const StateVector& xHat) { m_xHat = xHat; }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  void SetXhat(int i, double value) { m_xHat(i) = value; }

  /**
   * Resets the observer.
   */
  void Reset() {
    m_xHat.setZero();
    m_P = m_initP;
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const InputVector& u, units::second_t dt);

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const InputVector& u, const OutputVector& y) {
    Correct<Outputs>(u, y, m_h, m_contR, m_residualFuncY, m_addFuncX);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurement noise covariances vary.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  void Correct(const InputVector& u, const OutputVector& y,
               const Matrixd<Outputs, Outputs>& R) {
    Correct<Outputs>(u, y, m_h, R, m_residualFuncY, m_addFuncX);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement
   *          vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  template <int Rows>
  void Correct(
      const InputVector& u, const Vectord<Rows>& y,
      std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
      const Matrixd<Rows, Rows>& R);

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u             Same control input used in the predict step.
   * @param y             Measurement vector.
   * @param h             A vector-valued function of x and u that returns
   *                      the measurement vector.
   * @param R             Continuous measurement noise covariance matrix.
   * @param residualFuncY A function that computes the residual of two
   *                      measurement vectors (i.e. it subtracts them.)
   * @param addFuncX      A function that adds two state vectors.
   */
  template <int Rows>
  void Correct(
      const InputVector& u, const Vectord<Rows>& y,
      std::function<Vectord<Rows>(const StateVector&, const InputVector&)> h,
      const Matrixd<Rows, Rows>& R,
      std::function<Vectord<Rows>(const Vectord<Rows>&, const Vectord<Rows>&)>
          residualFuncY,
      std::function<StateVector(const StateVector&, const StateVector&)>
          addFuncX);

 private:
  std::function<StateVector(const StateVector&, const InputVector&)> m_f;
  std::function<OutputVector(const StateVector&, const InputVector&)> m_h;
  std::function<OutputVector(const OutputVector&, const OutputVector&)>
      m_residualFuncY;
  std::function<StateVector(const StateVector&, const StateVector&)> m_addFuncX;
  StateVector m_xHat;
  StateMatrix m_P;
  StateMatrix m_contQ;
  Matrixd<Outputs, Outputs> m_contR;
  units::second_t m_dt;

  StateMatrix m_initP;
};

}  // namespace frc

#include "ExtendedKalmanFilter.inc"
