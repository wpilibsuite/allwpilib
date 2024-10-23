// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <stdexcept>
#include <string>

#include <Eigen/Cholesky>
#include <wpi/SymbolExports.h>
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
 * This class assumes predict() and correct() are called in pairs, so the Kalman
 * gain converges to a steady-state value. If they aren't, use KalmanFilter
 * instead.
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
class SteadyStateKalmanFilter {
 public:
  using StateVector = Vectord<States>;
  using InputVector = Vectord<Inputs>;
  using OutputVector = Vectord<Outputs>;

  using StateArray = wpi::array<double, States>;
  using OutputArray = wpi::array<double, Outputs>;

  /**
   * Constructs a steady-state Kalman filter with the given plant.
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
  SteadyStateKalmanFilter(LinearSystem<States, Inputs, Outputs>& plant,
                          const StateArray& stateStdDevs,
                          const OutputArray& measurementStdDevs,
                          units::second_t dt) {
    m_plant = &plant;

    auto contQ = MakeCovMatrix(stateStdDevs);
    auto contR = MakeCovMatrix(measurementStdDevs);

    Matrixd<States, States> discA;
    Matrixd<States, States> discQ;
    DiscretizeAQ<States>(plant.A(), contQ, dt, &discA, &discQ);

    auto discR = DiscretizeR<Outputs>(contR, dt);

    const auto& C = plant.C();

    if (!IsDetectable<States, Outputs>(discA, C)) {
      std::string msg = fmt::format(
          "The system passed to the Kalman filter is undetectable!\n\n"
          "A =\n{}\nC =\n{}\n",
          discA, C);

      wpi::math::MathSharedStore::ReportError(msg);
      throw std::invalid_argument(msg);
    }

    Matrixd<States, States> P =
        DARE<States, Outputs>(discA.transpose(), C.transpose(), discQ, discR);

    // S = CPCᵀ + R
    Matrixd<Outputs, Outputs> S = C * P * C.transpose() + discR;

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
    m_K = S.transpose().ldlt().solve(C * P.transpose()).transpose();

    Reset();
  }

  SteadyStateKalmanFilter(SteadyStateKalmanFilter&&) = default;
  SteadyStateKalmanFilter& operator=(SteadyStateKalmanFilter&&) = default;

  /**
   * Returns the steady-state Kalman gain matrix K.
   */
  const Matrixd<States, Outputs>& K() const { return m_K; }

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
  void Reset() { m_xHat.setZero(); }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u  New control input from controller.
   * @param dt Timestep for prediction.
   */
  void Predict(const InputVector& u, units::second_t dt) {
    m_xHat = m_plant->CalculateX(m_xHat, u, dt);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the last predict step.
   * @param y Measurement vector.
   */
  void Correct(const InputVector& u, const OutputVector& y) {
    const auto& C = m_plant->C();
    const auto& D = m_plant->D();

    // x̂ₖ₊₁⁺ = x̂ₖ₊₁⁻ + K(y − (Cx̂ₖ₊₁⁻ + Duₖ₊₁))
    m_xHat += m_K * (y - (C * m_xHat + D * u));
  }

 private:
  LinearSystem<States, Inputs, Outputs>* m_plant;

  /**
   * The steady-state Kalman gain matrix.
   */
  Matrixd<States, Outputs> m_K;

  /**
   * The state estimate.
   */
  StateVector m_xHat;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SteadyStateKalmanFilter<1, 1, 1>;
extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SteadyStateKalmanFilter<2, 1, 1>;

}  // namespace frc
