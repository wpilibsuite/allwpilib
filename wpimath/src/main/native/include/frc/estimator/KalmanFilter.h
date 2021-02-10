// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>

#include <wpi/array.h>

#include "Eigen/Core"
#include "Eigen/src/Cholesky/LDLT.h"
#include "drake/math/discrete_algebraic_riccati_equation.h"
#include "frc/StateSpaceUtil.h"
#include "frc/system/Discretization.h"
#include "frc/system/LinearSystem.h"
#include "units/time.h"
#include "wpimath/MathShared.h"

namespace frc {
namespace detail {

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
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9
 * "Stochastic control theory".
 */
template <int States, int Inputs, int Outputs>
class KalmanFilterImpl {
 public:
  /**
   * Constructs a state-space observer with the given plant.
   *
   * @param plant              The plant used for the prediction step.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  KalmanFilterImpl(LinearSystem<States, Inputs, Outputs>& plant,
                   const wpi::array<double, States>& stateStdDevs,
                   const wpi::array<double, Outputs>& measurementStdDevs,
                   units::second_t dt) {
    m_plant = &plant;

    auto contQ = MakeCovMatrix(stateStdDevs);
    auto contR = MakeCovMatrix(measurementStdDevs);

    Eigen::Matrix<double, States, States> discA;
    Eigen::Matrix<double, States, States> discQ;
    DiscretizeAQTaylor<States>(plant.A(), contQ, dt, &discA, &discQ);

    auto discR = DiscretizeR<Outputs>(contR, dt);

    const auto& C = plant.C();

    // IsStabilizable(A^T, C^T) will tell us if the system is observable.
    bool isObservable =
        IsStabilizable<States, Outputs>(discA.transpose(), C.transpose());
    if (!isObservable) {
      wpi::math::MathSharedStore::ReportError(
          "The system passed to the Kalman filter is not observable!");
      throw std::invalid_argument(
          "The system passed to the Kalman filter is not observable!");
    }

    Eigen::Matrix<double, States, States> P =
        drake::math::DiscreteAlgebraicRiccatiEquation(
            discA.transpose(), C.transpose(), discQ, discR);

    Eigen::Matrix<double, Outputs, Outputs> S = C * P * C.transpose() + discR;

    // We want to put K = PC^T S^-1 into Ax = b form so we can solve it more
    // efficiently.
    //
    // K = PC^T S^-1
    // KS = PC^T
    // (KS)^T = (PC^T)^T
    // S^T K^T = CP^T
    //
    // The solution of Ax = b can be found via x = A.solve(b).
    //
    // K^T = S^T.solve(CP^T)
    // K = (S^T.solve(CP^T))^T
    m_K = S.transpose().ldlt().solve(C * P.transpose()).transpose();

    Reset();
  }

  KalmanFilterImpl(KalmanFilterImpl&&) = default;
  KalmanFilterImpl& operator=(KalmanFilterImpl&&) = default;

  /**
   * Returns the steady-state Kalman gain matrix K.
   */
  const Eigen::Matrix<double, States, Outputs>& K() const { return m_K; }

  /**
   * Returns an element of the steady-state Kalman gain matrix K.
   *
   * @param i Row of K.
   * @param j Column of K.
   */
  double K(int i, int j) const { return m_K(i, j); }

  /**
   * Returns the state estimate x-hat.
   */
  const Eigen::Matrix<double, States, 1>& Xhat() const { return m_xHat; }

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
  void SetXhat(const Eigen::Matrix<double, States, 1>& xHat) { m_xHat = xHat; }

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
  void Reset() { m_xHat.setZero(); }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
    m_xHat = m_plant->CalculateX(m_xHat, u, dt);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the last predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Outputs, 1>& y) {
    m_xHat += m_K * (y - (m_plant->C() * m_xHat + m_plant->D() * u));
  }

 private:
  LinearSystem<States, Inputs, Outputs>* m_plant;

  /**
   * The steady-state Kalman gain matrix.
   */
  Eigen::Matrix<double, States, Outputs> m_K;

  /**
   * The state estimate.
   */
  Eigen::Matrix<double, States, 1> m_xHat;
};

}  // namespace detail

template <int States, int Inputs, int Outputs>
class KalmanFilter : public detail::KalmanFilterImpl<States, Inputs, Outputs> {
 public:
  /**
   * Constructs a state-space observer with the given plant.
   *
   * @param plant              The plant used for the prediction step.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   */
  KalmanFilter(LinearSystem<States, Inputs, Outputs>& plant,
               const wpi::array<double, States>& stateStdDevs,
               const wpi::array<double, Outputs>& measurementStdDevs,
               units::second_t dt)
      : detail::KalmanFilterImpl<States, Inputs, Outputs>{
            plant, stateStdDevs, measurementStdDevs, dt} {}

  KalmanFilter(KalmanFilter&&) = default;
  KalmanFilter& operator=(KalmanFilter&&) = default;
};

// Template specializations are used here to make common state-input-output
// triplets compile faster.
template <>
class KalmanFilter<1, 1, 1> : public detail::KalmanFilterImpl<1, 1, 1> {
 public:
  KalmanFilter(LinearSystem<1, 1, 1>& plant,
               const wpi::array<double, 1>& stateStdDevs,
               const wpi::array<double, 1>& measurementStdDevs,
               units::second_t dt);

  KalmanFilter(KalmanFilter&&) = default;
  KalmanFilter& operator=(KalmanFilter&&) = default;
};

// Template specializations are used here to make common state-input-output
// triplets compile faster.
template <>
class KalmanFilter<2, 1, 1> : public detail::KalmanFilterImpl<2, 1, 1> {
 public:
  KalmanFilter(LinearSystem<2, 1, 1>& plant,
               const wpi::array<double, 2>& stateStdDevs,
               const wpi::array<double, 1>& measurementStdDevs,
               units::second_t dt);

  KalmanFilter(KalmanFilter&&) = default;
  KalmanFilter& operator=(KalmanFilter&&) = default;
};

}  // namespace frc
