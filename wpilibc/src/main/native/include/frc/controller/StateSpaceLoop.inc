/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <utility>

namespace frc {

template <int States, int Inputs, int Outputs>
StateSpaceLoop<States, Inputs, Outputs>::StateSpaceLoop(
    const StateSpacePlantCoeffs<States, Inputs, Outputs>& plantCoeffs,
    const StateSpaceControllerCoeffs<States, Inputs, Outputs>& controllerCoeffs,
    const StateSpaceObserverCoeffs<States, Inputs, Outputs>& observerCoeffs)
    : m_plant(plantCoeffs),
      m_controller(controllerCoeffs, m_plant),
      m_observer(observerCoeffs, m_plant) {
  Reset();
}

template <int States, int Inputs, int Outputs>
StateSpaceLoop<States, Inputs, Outputs>::StateSpaceLoop(
    StateSpacePlant<States, Inputs, Outputs>&& plant,
    StateSpaceController<States, Inputs, Outputs>&& controller,
    StateSpaceObserver<States, Inputs, Outputs>&& observer)
    : m_plant(std::move(plant)),
      m_controller(std::move(controller)),
      m_observer(std::move(observer)) {
  Reset();
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::Enable() {
  m_controller.Enable();
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::Disable() {
  m_controller.Disable();
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
StateSpaceLoop<States, Inputs, Outputs>::Xhat() const {
  return m_observer.Xhat();
}

template <int States, int Inputs, int Outputs>
double StateSpaceLoop<States, Inputs, Outputs>::Xhat(int i) const {
  return m_observer.Xhat(i);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>&
StateSpaceLoop<States, Inputs, Outputs>::NextR() const {
  return m_nextR;
}

template <int States, int Inputs, int Outputs>
double StateSpaceLoop<States, Inputs, Outputs>::NextR(int i) const {
  return NextR()(i, 0);
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, Inputs, 1>&
StateSpaceLoop<States, Inputs, Outputs>::U() const {
  return m_controller.U();
}

template <int States, int Inputs, int Outputs>
double StateSpaceLoop<States, Inputs, Outputs>::U(int i) const {
  return m_controller.U(i);
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::SetXhat(
    const Eigen::Matrix<double, States, 1>& xHat) {
  m_observer.SetXhat(xHat);
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::SetXhat(int i, double value) {
  m_observer.SetXhat(i, value);
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::SetNextR(
    const Eigen::Matrix<double, States, 1>& nextR) {
  m_nextR = nextR;
}

template <int States, int Inputs, int Outputs>
const StateSpacePlant<States, Inputs, Outputs>&
StateSpaceLoop<States, Inputs, Outputs>::GetPlant() const {
  return m_plant;
}

template <int States, int Inputs, int Outputs>
const StateSpaceController<States, Inputs, Outputs>&
StateSpaceLoop<States, Inputs, Outputs>::GetController() const {
  return m_controller;
}

template <int States, int Inputs, int Outputs>
const StateSpaceObserver<States, Inputs, Outputs>&
StateSpaceLoop<States, Inputs, Outputs>::GetObserver() const {
  return m_observer;
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::Reset() {
  m_plant.Reset();
  m_controller.Reset();
  m_observer.Reset();
  m_nextR.setZero();
}

template <int States, int Inputs, int Outputs>
const Eigen::Matrix<double, States, 1>
StateSpaceLoop<States, Inputs, Outputs>::Error() const {
  return m_controller.R() - m_observer.Xhat();
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::Correct(
    const Eigen::Matrix<double, Outputs, 1>& y) {
  m_observer.Correct(m_controller.U(), y);
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::Predict() {
  m_controller.Update(m_observer.Xhat(), m_nextR);
  m_observer.Predict(m_controller.U());
}

template <int States, int Inputs, int Outputs>
void StateSpaceLoop<States, Inputs, Outputs>::SetIndex(int index) {
  m_plant.SetIndex(index);
  m_controller.SetIndex(index);
  m_observer.SetIndex(index);
}

template <int States, int Inputs, int Outputs>
int StateSpaceLoop<States, Inputs, Outputs>::GetIndex() const {
  return m_plant.GetIndex();
}

}  // namespace frc
