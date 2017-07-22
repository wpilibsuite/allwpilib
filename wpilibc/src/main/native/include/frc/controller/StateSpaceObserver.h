/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <vector>

#include <Eigen/Core>

#include "frc/controller/StateSpaceObserverCoeffs.h"
#include "frc/controller/StateSpacePlant.h"

namespace frc {

/**
 * Luenberger observers combine predictions from a model and measurements to
 * give an estimate of the true system state.
 *
 * Luenberger observers use an L gain matrix to determine whether to trust the
 * model or measurements more. Kalman filter theory uses statistics to compute
 * an optimal L gain (alternatively called the Kalman gain, K) which minimizes
 * the sum of squares error in the state estimate.
 *
 * Luenberger observers run the prediction and correction steps simultaneously
 * while Kalman filters run them sequentially. To implement a discrete-time
 * Kalman filter as a Luenberger observer, use the following mapping:
 * <pre>C = H, L = A * K</pre>
 * (H is the measurement matrix).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
template <int States, int Inputs, int Outputs>
class StateSpaceObserver {
 public:
  /**
   * Constructs a state-space observer with the given coefficients and plant.
   *
   * @param observerCoeffs Observer coefficients.
   * @param plant          The plant used for the prediction step.
   */
  StateSpaceObserver(
      const StateSpaceObserverCoeffs<States, Inputs, Outputs>& observerCoeffs,
      StateSpacePlant<States, Inputs, Outputs>& plant);

  StateSpaceObserver(StateSpaceObserver&&) = default;
  StateSpaceObserver& operator=(StateSpaceObserver&&) = default;

  /**
   * Returns the Kalman gain matrix K.
   */
  const Eigen::Matrix<double, States, Outputs>& K() const;

  /**
   * Returns an element of the Kalman gain matrix K.
   *
   * @param i Row of K.
   * @param j Column of K.
   */
  double K(int i, int j) const;

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
  void Predict(const Eigen::Matrix<double, Inputs, 1>& newU);

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
      const StateSpaceObserverCoeffs<States, Inputs, Outputs>& coefficients);

  /**
   * Returns the observer coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  const StateSpaceObserverCoeffs<States, Inputs, Outputs>& GetCoefficients(
      int index) const;

  /**
   * Returns the current observer coefficients.
   */
  const StateSpaceObserverCoeffs<States, Inputs, Outputs>& GetCoefficients()
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
  StateSpacePlant<States, Inputs, Outputs>* m_plant;

  /**
   * Internal state estimate.
   */
  Eigen::Matrix<double, States, 1> m_Xhat;

  std::vector<StateSpaceObserverCoeffs<States, Inputs, Outputs>> m_coefficients;
  int m_index = 0;
};

}  // namespace frc

#include "frc/controller/StateSpaceObserver.inc"
