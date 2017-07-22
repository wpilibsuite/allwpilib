/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cassert>

#include <unsupported/Eigen/MatrixFunctions>

namespace frc {

template <int States, int Inputs, int Outputs>
PeriodVariantPlant<States, Inputs, Outputs>::PeriodVariantPlant(
    const PeriodVariantPlantCoeffs<States, Inputs, Outputs>& plantCoeffs,
    const units::second_t nominalSamplePeriod)
    : m_nominalSamplePeriod(nominalSamplePeriod) {
  AddCoefficients(plantCoeffs);
  Reset();
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, States>&
PeriodVariantPlant<States, Inputs, Outputs>::A() const {
  return m_A;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantPlant<States, Inputs, Outputs>::A(int i, int j) const {
  return A()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, Inputs>&
PeriodVariantPlant<States, Inputs, Outputs>::B() const {
  return m_B;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantPlant<States, Inputs, Outputs>::B(int i, int j) const {
  return B()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Outputs, States>&
PeriodVariantPlant<States, Inputs, Outputs>::C() const {
  return GetCoefficients().C;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantPlant<States, Inputs, Outputs>::C(int i, int j) const {
  return C()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Outputs, Inputs>&
PeriodVariantPlant<States, Inputs, Outputs>::D() const {
  return GetCoefficients().D;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantPlant<States, Inputs, Outputs>::D(int i, int j) const {
  return D()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
PeriodVariantPlant<States, Inputs, Outputs>::X() const {
  return m_X;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantPlant<States, Inputs, Outputs>::X(int i) const {
  return X()(i, 0);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Outputs, 1>&
PeriodVariantPlant<States, Inputs, Outputs>::Y() const {
  return m_Y;
}

template <int States, int Inputs, int Outputs>
double PeriodVariantPlant<States, Inputs, Outputs>::Y(int i) const {
  return Y()(i, 0);
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::SetX(
    const Eigen::Matrix<double, States, 1>& x) {
  m_X = x;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::SetX(int i, double value) {
  m_X(i, 0) = value;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::SetY(
    const Eigen::Matrix<double, Outputs, 1>& y) {
  m_Y = y;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::SetY(int i, double value) {
  m_Y(i, 0) = value;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::AddCoefficients(
    const PeriodVariantPlantCoeffs<States, Inputs, Outputs>& coefficients) {
  m_coefficients.emplace_back(coefficients);
}

template <int States, int Inputs, int Outputs>
const PeriodVariantPlantCoeffs<States, Inputs, Outputs>&
PeriodVariantPlant<States, Inputs, Outputs>::GetCoefficients(int index) const {
  return m_coefficients[index];
}

template <int States, int Inputs, int Outputs>
const PeriodVariantPlantCoeffs<States, Inputs, Outputs>&
PeriodVariantPlant<States, Inputs, Outputs>::GetCoefficients() const {
  return m_coefficients[m_index];
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::SetIndex(int index) {
  assert(index >= 0);
  assert(index < static_cast<int>(m_coefficients.size()));
  m_index = index;
}

template <int States, int Inputs, int Outputs>
int PeriodVariantPlant<States, Inputs, Outputs>::GetIndex() const {
  return m_index;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::Reset() {
  m_X.setZero();
  m_Y.setZero();
  m_A.setZero();
  m_B.setZero();
  m_delayedU.setZero();
  UpdateAB(m_nominalSamplePeriod);
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::Update(
    const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
  m_X = CalculateX(X(), m_delayedU);
  m_Y = CalculateY(X(), m_delayedU);
  m_delayedU = u;
}

template <int States, int Inputs, int Outputs>
Eigen::Matrix<double, States, 1>
PeriodVariantPlant<States, Inputs, Outputs>::CalculateX(
    const Eigen::Matrix<double, States, 1>& x,
    const Eigen::Matrix<double, Inputs, 1>& u, units::second_t dt) {
  UpdateAB(dt);
  return A() * x + B() * u;
}

template <int States, int Inputs, int Outputs>
Eigen::Matrix<double, Outputs, 1>
PeriodVariantPlant<States, Inputs, Outputs>::CalculateY(
    const Eigen::Matrix<double, States, 1>& x,
    const Eigen::Matrix<double, Inputs, 1>& u) const {
  return C() * x + D() * u;
}

template <int States, int Inputs, int Outputs>
void PeriodVariantPlant<States, Inputs, Outputs>::UpdateAB(units::second_t dt) {
  // Matrices are blocked here to minimize matrix exponentiation calculations.
  Eigen::Matrix<double, States + Inputs, States + Inputs> MstateContinuous;
  MstateContinuous.setZero();
  MstateContinuous.template block<States, States>(0, 0) =
      GetCoefficients().Acontinuous * dt.to<double>();
  MstateContinuous.template block<States, Inputs>(0, States) =
      GetCoefficients().Bcontinuous * dt.to<double>();

  Eigen::Matrix<double, States + Inputs, States + Inputs> Mstate =
      MstateContinuous.exp();
  m_A = Mstate.template block<States, States>(0, 0);
  m_B = Mstate.template block<States, Inputs>(0, States);
}

}  // namespace frc
