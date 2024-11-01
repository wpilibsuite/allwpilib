// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <stdexcept>
#include <string>

#include <Eigen/Cholesky>
#include <wpi/array.h>

#include "frc/DARE.h"
#include "frc/EigenCore.h"
#include "frc/StateSpaceUtil.h"
#include "frc/fmt/Eigen.h"
#include "frc/system/Discretization.h"
#include "frc/system/LinearSystem.h"
#include "units/time.h"
#include "wpimath/MathShared.h"

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
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9
 * "Stochastic control theory".
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @tparam Outputs Number of outputs.
 */
template <int States, int Inputs, int Outputs>
class KalmanFilter {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;
  using OutputVector = Vectord<Outputs>;

  using StateArray = wpi::array<double, States>;
  using OutputArray = wpi::array<double, Outputs>;

  using StateMatrix = Matrixd<States, States>;

  /**
   * Constructs a Kalman filter with the given plant.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices
   * for how to select the standard deviations.
   *
   * @param plant              The plant used for the prediction step.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dt                 Nominal discretization timestep.
   * @throws std::invalid_argument If the system is undetectable.
   */
  KalmanFilter(LinearSystem<States, Inputs, Outputs>& plant,
               const StateArray& stateStdDevs,
               const OutputArray& measurementStdDevs, units::second_t dt) {
    m_plant = &plant;

    m_contQ = MakeCovMatrix(stateStdDevs);
    m_contR = MakeCovMatrix(measurementStdDevs);
    m_dt = dt;

    // Find discrete A and Q
    Matrixd<States, States> discA;
    Matrixd<States, States> discQ;
    DiscretizeAQ<States>(plant.A(), m_contQ, dt, &discA, &discQ);

    Matrixd<Outputs, Outputs> discR = DiscretizeR<Outputs>(m_contR, dt);

    const auto& C = plant.C();

    if (auto P = DARE<States, Outputs>(discA.transpose(), C.transpose(), discQ,
                                       discR)) {
      m_initP = P.value();
    } else if (P.error() == DAREError::QNotSymmetric ||
               P.error() == DAREError::QNotPositiveSemidefinite) {
      std::string msg =
          fmt::format("{}\n\nQ =\n{}\n", to_string(P.error()), discQ);

      wpi::math::MathSharedStore::ReportError(msg);
      throw std::invalid_argument(msg);
    } else if (P.error() == DAREError::RNotSymmetric ||
               P.error() == DAREError::RNotPositiveDefinite) {
      std::string msg =
          fmt::format("{}\n\nR =\n{}\n", to_string(P.error()), discR);

      wpi::math::MathSharedStore::ReportError(msg);
      throw std::invalid_argument(msg);
    } else if (P.error() == DAREError::ABNotStabilizable) {
      std::string msg = fmt::format(
          "The (A, C) pair is not detectable.\n\nA =\n{}\nC =\n{}\n",
          to_string(P.error()), discA, C);

      wpi::math::MathSharedStore::ReportError(msg);
      throw std::invalid_argument(msg);
    } else if (P.error() == DAREError::ACNotDetectable) {
      std::string msg = fmt::format("{}\n\nA =\n{}\nQ =\n{}\n",
                                    to_string(P.error()), discA, discQ);

      wpi::math::MathSharedStore::ReportError(msg);
      throw std::invalid_argument(msg);
    }

    Reset();
  }

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
  void Predict(const InputVector& u, units::second_t dt) {
    // Find discrete A and Q
    StateMatrix discA;
    StateMatrix discQ;
    DiscretizeAQ<States>(m_plant->A(), m_contQ, dt, &discA, &discQ);

    m_xHat = m_plant->CalculateX(m_xHat, u, dt);

    // Pₖ₊₁⁻ = APₖ⁻Aᵀ + Q
    m_P = discA * m_P * discA.transpose() + discQ;

    m_dt = dt;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const InputVector& u, const OutputVector& y) {
    Correct(u, y, m_contR);
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
    const auto& C = m_plant->C();
    const auto& D = m_plant->D();

    const Matrixd<Outputs, Outputs> discR = DiscretizeR<Outputs>(R, m_dt);

    Matrixd<Outputs, Outputs> S = C * m_P * C.transpose() + discR;

    // We want to put K = PCᵀS⁻¹ into Ax = b form so we can solve it more
    // efficiently.
    //
    // K = PCᵀS⁻¹
    // KS = PCᵀ
    // (KS)ᵀ = (PCᵀ)ᵀ
    // SᵀKᵀ = CPᵀ
    //
    // The solution of Ax = b can be found via x = A.solve(b).
    //
    // Kᵀ = Sᵀ.solve(CPᵀ)
    // K = (Sᵀ.solve(CPᵀ))ᵀ
    Matrixd<States, Outputs> K =
        S.transpose().ldlt().solve(C * m_P.transpose()).transpose();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − (Cx̂ₖ₊₁⁻ + Duₖ₊₁))
    m_xHat += K * (y - (C * m_xHat + D * u));

    // Pₖ₊₁⁺ = (I−Kₖ₊₁C)Pₖ₊₁⁻(I−Kₖ₊₁C)ᵀ + Kₖ₊₁RKₖ₊₁ᵀ
    // Use Joseph form for numerical stability
    m_P = (StateMatrix::Identity() - K * C) * m_P *
              (StateMatrix::Identity() - K * C).transpose() +
          K * discR * K.transpose();
  }

 private:
  LinearSystem<States, Inputs, Outputs>* m_plant;
  StateVector m_xHat;
  StateMatrix m_P;
  StateMatrix m_contQ;
  Matrixd<Outputs, Outputs> m_contR;
  units::second_t m_dt;

  StateMatrix m_initP;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    KalmanFilter<1, 1, 1>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    KalmanFilter<2, 1, 1>;

}  // namespace frc
