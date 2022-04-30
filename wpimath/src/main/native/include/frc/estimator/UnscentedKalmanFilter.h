// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>

#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "Eigen/Core"
#include "frc/estimator/MerweScaledSigmaPoints.h"
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
 * An unscented Kalman filter uses nonlinear state and measurement models. It
 * propagates the error covariance using sigma points chosen to approximate the
 * true probability distribution.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9
 * "Stochastic control theory".
 *
 * @tparam States The number of states.
 * @tparam Inputs The number of inputs.
 * @tparam Outputs The number of outputs.
 */
template <int States, int Inputs, int Outputs>
class UnscentedKalmanFilter {
 public:
  /**
   * Constructs an unscented Kalman filter.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  UnscentedKalmanFilter(std::function<Eigen::Vector<double, States>(
                            const Eigen::Vector<double, States>&,
                            const Eigen::Vector<double, Inputs>&)>
                            f,
                        std::function<Eigen::Vector<double, Outputs>(
                            const Eigen::Vector<double, States>&,
                            const Eigen::Vector<double, Inputs>&)>
                            h,
                        const wpi::array<double, States>& stateStdDevs,
                        const wpi::array<double, Outputs>& measurementStdDevs,
                        units::second_t dt);

  /**
   * Constructs an unscented Kalman filter with custom mean, residual, and
   * addition functions. Using custom functions for arithmetic can be useful if
   * you have angles in the state or measurements, because they allow you to
   * correctly account for the modular nature of angle arithmetic.
   *
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param meanFuncX          A function that computes the mean of 2 * States +
   *                           1 state vectors using a given set of weights.
   * @param meanFuncY          A function that computes the mean of 2 * States +
   *                           1 measurement vectors using a given set of
   *                           weights.
   * @param residualFuncX      A function that computes the residual of two
   *                           state vectors (i.e. it subtracts them.)
   * @param residualFuncY      A function that computes the residual of two
   *                           measurement vectors (i.e. it subtracts them.)
   * @param addFuncX           A function that adds two state vectors.
   * @param dt                 Nominal discretization timestep.
   */
  UnscentedKalmanFilter(
      std::function<
          Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                        const Eigen::Vector<double, Inputs>&)>
          f,
      std::function<
          Eigen::Vector<double, Outputs>(const Eigen::Vector<double, States>&,
                                         const Eigen::Vector<double, Inputs>&)>
          h,
      const wpi::array<double, States>& stateStdDevs,
      const wpi::array<double, Outputs>& measurementStdDevs,
      std::function<Eigen::Vector<double, States>(
          const Eigen::Matrix<double, States, 2 * States + 1>&,
          const Eigen::Vector<double, 2 * States + 1>&)>
          meanFuncX,
      std::function<Eigen::Vector<double, Outputs>(
          const Eigen::Matrix<double, Outputs, 2 * States + 1>&,
          const Eigen::Vector<double, 2 * States + 1>&)>
          meanFuncY,
      std::function<
          Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                        const Eigen::Vector<double, States>&)>
          residualFuncX,
      std::function<
          Eigen::Vector<double, Outputs>(const Eigen::Vector<double, Outputs>&,
                                         const Eigen::Vector<double, Outputs>&)>
          residualFuncY,
      std::function<
          Eigen::Vector<double, States>(const Eigen::Vector<double, States>&,
                                        const Eigen::Vector<double, States>&)>
          addFuncX,
      units::second_t dt);

  /**
   * Returns the error covariance matrix P.
   */
  const Eigen::Matrix<double, States, States>& P() const { return m_P; }

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
  void SetP(const Eigen::Matrix<double, States, States>& P) { m_P = P; }

  /**
   * Returns the state estimate x-hat.
   */
  const Eigen::Vector<double, States>& Xhat() const { return m_xHat; }

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
  void SetXhat(const Eigen::Vector<double, States>& xHat) { m_xHat = xHat; }

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
    m_P.setZero();
    m_sigmasF.setZero();
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const Eigen::Vector<double, Inputs>& u, units::second_t dt);

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Vector<double, Inputs>& u,
               const Eigen::Vector<double, Outputs>& y) {
    Correct<Outputs>(u, y, m_h, m_contR, m_meanFuncY, m_residualFuncY,
                     m_residualFuncX, m_addFuncX);
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
   * @param R Measurement noise covariance matrix (continuous-time).
   */
  template <int Rows>
  void Correct(const Eigen::Vector<double, Inputs>& u,
               const Eigen::Vector<double, Rows>& y,
               std::function<Eigen::Vector<double, Rows>(
                   const Eigen::Vector<double, States>&,
                   const Eigen::Vector<double, Inputs>&)>
                   h,
               const Eigen::Matrix<double, Rows, Rows>& R);

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u             Same control input used in the predict step.
   * @param y             Measurement vector.
   * @param h             A vector-valued function of x and u that returns the
   *                      measurement vector.
   * @param R             Measurement noise covariance matrix (continuous-time).
   * @param meanFuncY     A function that computes the mean of 2 * States + 1
   *                      measurement vectors using a given set of weights.
   * @param residualFuncY A function that computes the residual of two
   *                      measurement vectors (i.e. it subtracts them.)
   * @param residualFuncX A function that computes the residual of two state
   *                      vectors (i.e. it subtracts them.)
   * @param addFuncX      A function that adds two state vectors.
   */
  template <int Rows>
  void Correct(const Eigen::Vector<double, Inputs>& u,
               const Eigen::Vector<double, Rows>& y,
               std::function<Eigen::Vector<double, Rows>(
                   const Eigen::Vector<double, States>&,
                   const Eigen::Vector<double, Inputs>&)>
                   h,
               const Eigen::Matrix<double, Rows, Rows>& R,
               std::function<Eigen::Vector<double, Rows>(
                   const Eigen::Matrix<double, Rows, 2 * States + 1>&,
                   const Eigen::Vector<double, 2 * States + 1>&)>
                   meanFuncY,
               std::function<Eigen::Vector<double, Rows>(
                   const Eigen::Vector<double, Rows>&,
                   const Eigen::Vector<double, Rows>&)>
                   residualFuncY,
               std::function<Eigen::Vector<double, States>(
                   const Eigen::Vector<double, States>&,
                   const Eigen::Vector<double, States>&)>
                   residualFuncX,
               std::function<Eigen::Vector<double, States>(
                   const Eigen::Vector<double, States>&,
                   const Eigen::Vector<double, States>)>
                   addFuncX);

 private:
  std::function<Eigen::Vector<double, States>(
      const Eigen::Vector<double, States>&,
      const Eigen::Vector<double, Inputs>&)>
      m_f;
  std::function<Eigen::Vector<double, Outputs>(
      const Eigen::Vector<double, States>&,
      const Eigen::Vector<double, Inputs>&)>
      m_h;
  std::function<Eigen::Vector<double, States>(
      const Eigen::Matrix<double, States, 2 * States + 1>&,
      const Eigen::Vector<double, 2 * States + 1>&)>
      m_meanFuncX;
  std::function<Eigen::Vector<double, Outputs>(
      const Eigen::Matrix<double, Outputs, 2 * States + 1>&,
      const Eigen::Vector<double, 2 * States + 1>&)>
      m_meanFuncY;
  std::function<Eigen::Vector<double, States>(
      const Eigen::Vector<double, States>&,
      const Eigen::Vector<double, States>&)>
      m_residualFuncX;
  std::function<Eigen::Vector<double, Outputs>(
      const Eigen::Vector<double, Outputs>&,
      const Eigen::Vector<double, Outputs>)>
      m_residualFuncY;
  std::function<Eigen::Vector<double, States>(
      const Eigen::Vector<double, States>&,
      const Eigen::Vector<double, States>)>
      m_addFuncX;
  Eigen::Vector<double, States> m_xHat;
  Eigen::Matrix<double, States, States> m_P;
  Eigen::Matrix<double, States, States> m_contQ;
  Eigen::Matrix<double, Outputs, Outputs> m_contR;
  Eigen::Matrix<double, States, 2 * States + 1> m_sigmasF;
  units::second_t m_dt;

  MerweScaledSigmaPoints<States> m_pts;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<3, 3, 1>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    UnscentedKalmanFilter<5, 3, 3>;

}  // namespace frc

#include "UnscentedKalmanFilter.inc"
