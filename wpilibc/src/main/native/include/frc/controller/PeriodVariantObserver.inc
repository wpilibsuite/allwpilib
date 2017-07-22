/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <unsupported/Eigen/MatrixFunctions>

namespace frc {

template <int States, int Inputs, int Outputs>
PeriodVariantObserver<States, Inputs, Outputs>::PeriodVariantObserver(
    const PeriodVariantObserverCoeffs<States, Inputs, Outputs>& observerCoeffs,
    PeriodVariantPlant<States, Inputs, Outputs>& plant,
    const units::second_t nominalSamplePeriod)
    : m_plant(&plant), m_nominalSamplePeriod(nominalSamplePeriod) {
  AddCoefficients(observerCoeffs);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, States>&
PeriodVariantObserver<States, Inputs, Outputs>::Q() const {
  return m_Q;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantObserver<States, Inputs, Outputs>::Q(int i, int j) const {
  return Q()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Outputs, Outputs>&
PeriodVariantObserver<States, Inputs, Outputs>::R() const {
  return m_R;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantObserver<States, Inputs, Outputs>::R(int i, int j) const {
  return R()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, States>&
PeriodVariantObserver<States, Inputs, Outputs>::P() const {
  return m_P;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantObserver<States, Inputs, Outputs>::P(int i, int j) const {
  return P()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
PeriodVariantObserver<States, Inputs, Outputs>::Xhat() const {
  return m_Xhat;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantObserver<States, Inputs, Outputs>::Xhat(int i) const {
  return Xhat()(i, 0);
}

template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::SetXhat(
    const Eigen::Matrix<double, States, 1>& xHat) {
  m_Xhat = xHat;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::SetXhat(int i,
                                                             double value) {
  m_Xhat(i, 0) = value;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::Reset() {
  m_Xhat.setZero();
  m_P = GetCoefficients().PsteadyState;
  UpdateQR(m_nominalSamplePeriod);
}

template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::Predict(
    const Eigen::Matrix<double, Inputs, 1>& newU, units::second_t dt) {
  // Trigger the predict step. This will update A() and B() in the plant.
  m_Xhat = m_plant->CalculateX(Xhat(), newU, dt);

  UpdateQR(dt);
  m_P = m_plant->A() * P() * m_plant->A().transpose() + Q();
}

template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::Correct(
    const Eigen::Matrix<double, Inputs, 1>& u,
    const Eigen::Matrix<double, Outputs, 1>& y) {
  Eigen::Matrix<double, Outputs, 1> yBar = y - m_plant->CalculateY(Xhat(), u);
  Eigen::Matrix<double, Outputs, Outputs> S =
      m_plant->C() * P() * m_plant->C().transpose() + R();

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
  Eigen::Matrix<double, States, Outputs> kalmanGain =
      S.transpose().ldlt().solve(m_plant->C() * P().transpose()).transpose();
  m_Xhat = Xhat() + kalmanGain * yBar;
  m_P = (m_plant->GetCoefficients().Acontinuous.Identity() -
         kalmanGain * m_plant->C()) *
        P();
}

template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::AddCoefficients(
    const PeriodVariantObserverCoeffs<States, Inputs, Outputs>& coefficients) {
  m_coefficients.emplace_back(coefficients);
}

template <int States, int Inputs, int Outputs>
const PeriodVariantObserverCoeffs<States, Inputs, Outputs>&
PeriodVariantObserver<States, Inputs, Outputs>::GetCoefficients(
    int index) const {
  return m_coefficients[index];
}

template <int States, int Inputs, int Outputs>
const PeriodVariantObserverCoeffs<States, Inputs, Outputs>&
PeriodVariantObserver<States, Inputs, Outputs>::GetCoefficients() const {
  return m_coefficients[m_index];
}

template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::SetIndex(int index) {
  if (index < 0) {
    m_index = 0;
  } else if (index >= static_cast<int>(m_coefficients.size())) {
    m_index = static_cast<int>(m_coefficients.size()) - 1;
  } else {
    m_index = index;
  }
}

template <int States, int Inputs, int Outputs>
int PeriodVariantObserver<States, Inputs, Outputs>::GetIndex() const {
  return m_index;
}

/**
 * Rediscretizes Q and R matrices based on provided period.
 */
template <int States, int Inputs, int Outputs>
void PeriodVariantObserver<States, Inputs, Outputs>::UpdateQR(
    units::second_t dt) {
  // Now, compute the discrete time Q and R coefficients.
  Eigen::Matrix<double, States, States> Qtemp =
      (GetCoefficients().Qcontinuous +
       GetCoefficients().Qcontinuous.transpose()) /
      2.0;
  Eigen::Matrix<double, Outputs, Outputs> Rtemp =
      (GetCoefficients().Rcontinuous +
       GetCoefficients().Rcontinuous.transpose()) /
      2.0;

  Eigen::Matrix<double, 2 * States, 2 * States> Mgain;
  Mgain.setZero();

  // Set up the matrix M = [[-A, Q], [0, A.T]]
  Mgain.template block<States, States>(0, 0) =
      -m_plant->GetCoefficients().Acontinuous;
  Mgain.template block<States, States>(0, States) = Qtemp;
  Mgain.template block<States, States>(States, States) =
      m_plant->GetCoefficients().Acontinuous.transpose();

  Eigen::Matrix<double, 2 * States, 2 * States> phi =
      (Mgain * dt.to<double>()).exp();

  // Phi12 = phi[0:States,        States:2*States]
  // Phi22 = phi[States:2*States, States:2*States]
  Eigen::Matrix<double, States, States> phi12 =
      phi.block(0, States, States, States);
  Eigen::Matrix<double, States, States> phi22 =
      phi.block(States, States, States, States);

  m_Q = phi22.transpose() * phi12;
  m_Q = (m_Q + m_Q.transpose()) / 2.0;
  m_R = Rtemp / dt.to<double>();
}

}  // namespace frc
