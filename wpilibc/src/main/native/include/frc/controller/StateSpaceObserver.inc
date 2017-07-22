/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

template <int States, int Inputs, int Outputs>
StateSpaceObserver<States, Inputs, Outputs>::StateSpaceObserver(
    const StateSpaceObserverCoeffs<States, Inputs, Outputs>& observerCoeffs,
    StateSpacePlant<States, Inputs, Outputs>& plant)
    : m_plant(&plant) {
  AddCoefficients(observerCoeffs);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, Outputs>&
StateSpaceObserver<States, Inputs, Outputs>::K() const {
  return GetCoefficients().K;
}

template <int States, int Inputs, int Outputs>
double StateSpaceObserver<States, Inputs, Outputs>::K(int i, int j) const {
  return K()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
StateSpaceObserver<States, Inputs, Outputs>::Xhat() const {
  return m_Xhat;
}

template <int States, int Inputs, int Outputs>
double StateSpaceObserver<States, Inputs, Outputs>::Xhat(int i) const {
  return Xhat()(i, 0);
}

template <int States, int Inputs, int Outputs>
void StateSpaceObserver<States, Inputs, Outputs>::SetXhat(
    const Eigen::Matrix<double, States, 1>& xHat) {
  m_Xhat = xHat;
}

template <int States, int Inputs, int Outputs>
void StateSpaceObserver<States, Inputs, Outputs>::SetXhat(int i, double value) {
  m_Xhat(i, 0) = value;
}

template <int States, int Inputs, int Outputs>
void StateSpaceObserver<States, Inputs, Outputs>::Reset() {
  m_Xhat.setZero();
}

template <int States, int Inputs, int Outputs>
void StateSpaceObserver<States, Inputs, Outputs>::Predict(
    const Eigen::Matrix<double, Inputs, 1>& newU) {
  m_Xhat = m_plant->CalculateX(Xhat(), newU);
}

template <int States, int Inputs, int Outputs>
void StateSpaceObserver<States, Inputs, Outputs>::Correct(
    const Eigen::Matrix<double, Inputs, 1>& u,
    const Eigen::Matrix<double, Outputs, 1>& y) {
  m_Xhat += K() * (y - m_plant->CalculateY(Xhat(), u));
}

template <int States, int Inputs, int Outputs>
void StateSpaceObserver<States, Inputs, Outputs>::AddCoefficients(
    const StateSpaceObserverCoeffs<States, Inputs, Outputs>& coefficients) {
  m_coefficients.emplace_back(coefficients);
}

template <int States, int Inputs, int Outputs>
const StateSpaceObserverCoeffs<States, Inputs, Outputs>&
StateSpaceObserver<States, Inputs, Outputs>::GetCoefficients(int index) const {
  return m_coefficients[index];
}

template <int States, int Inputs, int Outputs>
const StateSpaceObserverCoeffs<States, Inputs, Outputs>&
StateSpaceObserver<States, Inputs, Outputs>::GetCoefficients() const {
  return m_coefficients[m_index];
}

template <int States, int Inputs, int Outputs>
void StateSpaceObserver<States, Inputs, Outputs>::SetIndex(int index) {
  if (index < 0) {
    m_index = 0;
  } else if (index >= static_cast<int>(m_coefficients.size())) {
    m_index = static_cast<int>(m_coefficients.size()) - 1;
  } else {
    m_index = index;
  }
}

template <int States, int Inputs, int Outputs>
int StateSpaceObserver<States, Inputs, Outputs>::GetIndex() const {
  return m_index;
}

}  // namespace frc
