/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <Eigen/Core>
#include <units/units.h>

#include "frc/controller/PeriodVariantObserverCoeffs.h"
#include "frc/controller/PeriodVariantPlant.h"

namespace frc {

/**
 * A Kalman filter that discretizes its model and covariance matrices after
 * every sample period.
 *
 * Typical discrete state feedback implementations discretize with a nominal
 * sample period offline. If the real system doesn't maintain this period, this
 * nonlinearity can negatively affect the state estimate. This class discretizes
 * the continuous model after each measurement based on the measured sample
 * period.
 *
 * Since the sample period changes during runtime, the process and measurement
 * noise covariance matrices as well as the true steady state error covariance
 * change as well. During runtime, the error covariance matrix is initialized
 * with the discrete steady state value, but is updated during runtime as well.
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
template <int States, int Inputs, int Outputs>
class PeriodVariantObserver {
 public:
  /**
   * Constructs a Kalman filter with the given coefficients and plant.
   *
   * @param observerCoeffs      Observer coefficients.
   * @param plant               The plant used for the prediction step.
   * @param nominalSamplePeriod The nominal period at which the control loop
   *                            will run.
   */
  PeriodVariantObserver(const PeriodVariantObserverCoeffs<
                            States, Inputs, Outputs>& observerCoeffs,
                        PeriodVariantPlant<States, Inputs, Outputs>& plant,
                        const units::second_t nominalSamplePeriod = 5_ms);

  PeriodVariantObserver(PeriodVariantObserver&&) = default;
  PeriodVariantObserver& operator=(PeriodVariantObserver&&) = default;

  /**
   * Returns the discretized process noise covariance matrix.
   */
  const Eigen::Matrix<double, States, States>& Q() const;

  /**
   * Returns an element of the discretized process noise covariance matrix.
   *
   * @param i Row in Q.
   * @param j Column in Q.
   */
  double Q(int i, int j) const;

  /**
   * Returns the discretized measurement noise covariance matrix.
   */
  const Eigen::Matrix<double, Outputs, Outputs>& R() const;

  /**
   * Returns an element of the discretized measurement noise covariance matrix.
   *
   * @param i Row of R.
   * @param j Column of R.
   */
  double R(int i, int j) const;

  /**
   * Returns the discretized error covariance matrix.
   */
  const Eigen::Matrix<double, States, States>& P() const;

  /**
   * Returns an element of the discretized error covariance matrix.
   *
   * @param i Row of P.
   * @param j Column of P.
   */
  double P(int i, int j) const;

  /**
   * Returns the state estimate x-hat.
   */
  const Eigen::Matrix<double, States, 1>& Xhat() const;

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const;

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  void SetXhat(const Eigen::Matrix<double, States, 1>& xHat);

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  void SetXhat(int i, double value);

  /**
   * Resets the observer.
   */
  void Reset();

  /**
   * Project the model into the future with a new control input u.
   *
   * @param newU New control input from controller.
   * @param dt   Timestep for prediction.
   */
  void Predict(const Eigen::Matrix<double, Inputs, 1>& newU,
               units::second_t dt);

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Inputs, 1>& u,
               const Eigen::Matrix<double, Outputs, 1>& y);

  /**
   * Adds the given coefficients to the observer for gain scheduling.
   *
   * @param coefficients Observer coefficients.
   */
  void AddCoefficients(
      const PeriodVariantObserverCoeffs<States, Inputs, Outputs>& coefficients);

  /**
   * Returns the observer coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  const PeriodVariantObserverCoeffs<States, Inputs, Outputs>& GetCoefficients(
      int index) const;

  /**
   * Returns the current observer coefficients.
   */
  const PeriodVariantObserverCoeffs<States, Inputs, Outputs>& GetCoefficients()
      const;

  /**
   * Sets the current observer to be "index", clamped to be within range. This
   * can be used for gain scheduling.
   *
   * @param index The controller index.
   */
  void SetIndex(int index);

  /**
   * Returns the current observer index.
   */
  int GetIndex() const;

 private:
  /**
   * Rediscretizes Q and R with the provided timestep.
   *
   * @param dt Discretization timestep.
   */
  void UpdateQR(units::second_t dt);

  PeriodVariantPlant<States, Inputs, Outputs>* m_plant;
  const units::second_t m_nominalSamplePeriod;

  /**
   * Internal state estimate.
   */
  Eigen::Matrix<double, States, 1> m_Xhat;

  /**
   * Internal error covariance estimate.
   */
  Eigen::Matrix<double, States, States> m_P;

  /**
   * Discretized process noise covariance matrix.
   */
  Eigen::Matrix<double, States, States> m_Q;

  /**
   * Discretized measurement noise covariance matrix.
   */
  Eigen::Matrix<double, Outputs, Outputs> m_R;

  std::vector<PeriodVariantObserverCoeffs<States, Inputs, Outputs>>
      m_coefficients;
  int m_index = 0;
};

}  // namespace frc

#include "frc/controller/PeriodVariantObserver.inc"
