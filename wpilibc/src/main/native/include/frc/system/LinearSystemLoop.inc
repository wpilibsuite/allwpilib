/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

template <int States, int Inputs, int Outputs>
LinearSystemLoop<States, Inputs, Outputs>::LinearSystemLoop(
    LinearSystem<States, Inputs, Outputs>& plant,
    LinearQuadraticRegulator<States, Inputs>& controller,
    KalmanFilter<States, Inputs, Outputs>& observer)
    : m_plant(plant), m_controller(controller), m_observer(observer) {
  Reset();
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::Enable() {
  m_controller.Enable();
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::Disable() {
  m_controller.Disable();
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
LinearSystemLoop<States, Inputs, Outputs>::Xhat() const {
  return m_observer.Xhat();
}

template <int States, int Inputs, int Outputs>
double LinearSystemLoop<States, Inputs, Outputs>::Xhat(int i) const {
  return m_observer.Xhat(i);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
LinearSystemLoop<States, Inputs, Outputs>::NextR() const {
  return m_nextR;
}

template <int States, int Inputs, int Outputs>
double LinearSystemLoop<States, Inputs, Outputs>::NextR(int i) const {
  return NextR()(i, 0);
}

template <int States, int Inputs, int Outputs>
Eigen::Matrix<double, Inputs, 1> LinearSystemLoop<States, Inputs, Outputs>::U()
    const {
  return m_plant.ClampInput(m_controller.U());
}

template <int States, int Inputs, int Outputs>
double LinearSystemLoop<States, Inputs, Outputs>::U(int i) const {
  return m_plant.ClampInput(m_controller.U())(i, 0);
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::SetXhat(
    const Eigen::Matrix<double, States, 1>& xHat) {
  m_observer.SetXhat(xHat);
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::SetXhat(int i, double value) {
  m_observer.SetXhat(i, value);
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::SetNextR(
    const Eigen::Matrix<double, States, 1>& nextR) {
  m_nextR = nextR;
}

template <int States, int Inputs, int Outputs>
const LinearSystem<States, Inputs, Outputs>&
LinearSystemLoop<States, Inputs, Outputs>::Plant() const {
  return m_plant;
}

template <int States, int Inputs, int Outputs>
const LinearQuadraticRegulator<States, Inputs>&
LinearSystemLoop<States, Inputs, Outputs>::Controller() const {
  return m_controller;
}

template <int States, int Inputs, int Outputs>
const KalmanFilter<States, Inputs, Outputs>&
LinearSystemLoop<States, Inputs, Outputs>::Observer() const {
  return m_observer;
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::Reset() {
  m_plant.Reset();
  m_controller.Reset();
  m_observer.Reset();
  m_nextR.setZero();
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>
LinearSystemLoop<States, Inputs, Outputs>::Error() const {
  return m_controller.R() - m_observer.Xhat();
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::Correct(
    const Eigen::Matrix<double, Outputs, 1>& y) {
  m_observer.Correct(m_controller.U(), y);
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::Predict(units::second_t dt) {
  m_controller.Update(m_observer.Xhat(), m_nextR);
  m_observer.Predict(m_controller.U(), dt);
}

template <int States, int Inputs, int Outputs>
void LinearSystemLoop<States, Inputs, Outputs>::SetIndex(int index) {
  m_plant.SetIndex(index);
  m_controller.SetIndex(index);
  m_observer.SetIndex(index);
}

template <int States, int Inputs, int Outputs>
int LinearSystemLoop<States, Inputs, Outputs>::GetIndex() const {
  return m_plant.GetIndex();
}

}  // namespace frc
