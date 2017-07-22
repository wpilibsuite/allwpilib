/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cassert>

namespace frc {

template <int States, int Inputs, int Outputs>
StateSpacePlant<States, Inputs, Outputs>::StateSpacePlant(
    const StateSpacePlantCoeffs<States, Inputs, Outputs>& plantCoeffs) {
  AddCoefficients(plantCoeffs);
  Reset();
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, States>&
StateSpacePlant<States, Inputs, Outputs>::A() const {
  return GetCoefficients().A;
}

template <int States, int Inputs, int Outputs>
double StateSpacePlant<States, Inputs, Outputs>::A(int i, int j) const {
  return A()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, Inputs>&
StateSpacePlant<States, Inputs, Outputs>::B() const {
  return GetCoefficients().B;
}

template <int States, int Inputs, int Outputs>
double StateSpacePlant<States, Inputs, Outputs>::B(int i, int j) const {
  return B()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Outputs, States>&
StateSpacePlant<States, Inputs, Outputs>::C() const {
  return GetCoefficients().C;
}

template <int States, int Inputs, int Outputs>
double StateSpacePlant<States, Inputs, Outputs>::C(int i, int j) const {
  return C()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Outputs, Inputs>&
StateSpacePlant<States, Inputs, Outputs>::D() const {
  return GetCoefficients().D;
}

template <int States, int Inputs, int Outputs>
double StateSpacePlant<States, Inputs, Outputs>::D(int i, int j) const {
  return D()(i, j);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
StateSpacePlant<States, Inputs, Outputs>::X() const {
  return m_X;
}

template <int States, int Inputs, int Outputs>
double StateSpacePlant<States, Inputs, Outputs>::X(int i) const {
  return X()(i, 0);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Outputs, 1>&
StateSpacePlant<States, Inputs, Outputs>::Y() const {
  return m_Y;
}

template <int States, int Inputs, int Outputs>
double StateSpacePlant<States, Inputs, Outputs>::Y(int i) const {
  return Y()(i, 0);
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::SetX(
    const Eigen::Matrix<double, States, 1>& x) {
  m_X = x;
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::SetX(int i, double value) {
  m_X(i, 0) = value;
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::SetY(
    const Eigen::Matrix<double, Outputs, 1>& y) {
  m_Y = y;
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::SetY(int i, double value) {
  m_Y(i, 0) = value;
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::AddCoefficients(
    const StateSpacePlantCoeffs<States, Inputs, Outputs>& coefficients) {
  m_coefficients.emplace_back(coefficients);
}

template <int States, int Inputs, int Outputs>
const StateSpacePlantCoeffs<States, Inputs, Outputs>&
StateSpacePlant<States, Inputs, Outputs>::GetCoefficients(int index) const {
  return m_coefficients[index];
}

template <int States, int Inputs, int Outputs>
const StateSpacePlantCoeffs<States, Inputs, Outputs>&
StateSpacePlant<States, Inputs, Outputs>::GetCoefficients() const {
  return m_coefficients[m_index];
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::SetIndex(int index) {
  assert(index >= 0);
  assert(index < static_cast<int>(m_coefficients.size()));
  m_index = index;
}

template <int States, int Inputs, int Outputs>
int StateSpacePlant<States, Inputs, Outputs>::GetIndex() const {
  return m_index;
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::Reset() {
  m_X.setZero();
  m_Y.setZero();
}

template <int States, int Inputs, int Outputs>
void StateSpacePlant<States, Inputs, Outputs>::Update(
    const Eigen::Matrix<double, States, 1>& x,
    const Eigen::Matrix<double, Inputs, 1>& u) {
  m_X = CalculateX(x, u);
  m_Y = CalculateY(x, u);
}

template <int States, int Inputs, int Outputs>
Eigen::Matrix<double, States, 1>
StateSpacePlant<States, Inputs, Outputs>::CalculateX(
    const Eigen::Matrix<double, States, 1>& x,
    const Eigen::Matrix<double, Inputs, 1>& u) const {
  return A() * x + B() * u;
}

template <int States, int Inputs, int Outputs>
Eigen::Matrix<double, Outputs, 1>
StateSpacePlant<States, Inputs, Outputs>::CalculateY(
    const Eigen::Matrix<double, States, 1>& x,
    const Eigen::Matrix<double, Inputs, 1>& u) const {
  return C() * x + D() * u;
}

}  // namespace frc
