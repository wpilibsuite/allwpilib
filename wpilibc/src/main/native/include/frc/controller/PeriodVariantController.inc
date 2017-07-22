/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

template <int States, int Inputs, int Outputs>
PeriodVariantController<States, Inputs, Outputs>::PeriodVariantController(
    const StateSpaceControllerCoeffs<States, Inputs, Outputs>& controllerCoeffs,
    PeriodVariantPlant<States, Inputs, Outputs>& plant)
    : m_plant(&plant) {
  AddCoefficients(controllerCoeffs);
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::Enable() {
  m_enabled = true;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::Disable() {
  m_enabled = false;
  m_u.setZero();
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Inputs, States>&
PeriodVariantController<States, Inputs, Outputs>::K() const {
  return GetCoefficients().K;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantController<States, Inputs, Outputs>::K(int i, int j) const {
  return K()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Inputs, States>&
PeriodVariantController<States, Inputs, Outputs>::Kff() const {
  return GetCoefficients().Kff;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantController<States, Inputs, Outputs>::Kff(int i,
                                                             int j) const {
  return Kff()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
PeriodVariantController<States, Inputs, Outputs>::R() const {
  return m_r;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantController<States, Inputs, Outputs>::R(int i) const {
  return R()(i, 0);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Inputs, 1>&
PeriodVariantController<States, Inputs, Outputs>::U() const {
  return m_u;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantController<States, Inputs, Outputs>::U(int i) const {
  return U()(i, 0);
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::Reset() {
  m_r.setZero();
  m_u.setZero();
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::Update(
    const Eigen::Matrix<double, States, 1>& x) {
  if (m_enabled) {
    m_u = K() * (m_r - x) + Kff() * (m_r - m_plant->A() * m_r);
    CapU();
  }
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::Update(
    const Eigen::Matrix<double, States, 1>& x,
    const Eigen::Matrix<double, States, 1>& nextR) {
  if (m_enabled) {
    m_u = K() * (m_r - x) + Kff() * (nextR - m_plant->A() * m_r);
    CapU();
    m_r = nextR;
  }
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::CapU() {
  for (int i = 0; i < Inputs; ++i) {
    if (U(i) > GetCoefficients().Umax(i)) {
      m_u(i, 0) = GetCoefficients().Umax(i);
    } else if (U(i) < GetCoefficients().Umin(i)) {
      m_u(i, 0) = GetCoefficients().Umin(i);
    }
  }
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::AddCoefficients(
    const StateSpaceControllerCoeffs<States, Inputs, Outputs>& coefficients) {
  m_coefficients.emplace_back(coefficients);
}

template <int States, int Inputs, int Outputs>
const StateSpaceControllerCoeffs<States, Inputs, Outputs>&
PeriodVariantController<States, Inputs, Outputs>::GetCoefficients(
    int index) const {
  return m_coefficients[index];
}

template <int States, int Inputs, int Outputs>
const StateSpaceControllerCoeffs<States, Inputs, Outputs>&
PeriodVariantController<States, Inputs, Outputs>::GetCoefficients() const {
  return m_coefficients[m_index];
}

template <int States, int Inputs, int Outputs>
void PeriodVariantController<States, Inputs, Outputs>::SetIndex(int index) {
  if (index < 0) {
    m_index = 0;
  } else if (index >= static_cast<int>(m_coefficients.size())) {
    m_index = static_cast<int>(m_coefficients.size()) - 1;
  } else {
    m_index = index;
  }
}

template <int States, int Inputs, int Outputs>
int PeriodVariantController<States, Inputs, Outputs>::GetIndex() const {
  return m_index;
}

}  // namespace frc
