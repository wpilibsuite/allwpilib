// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Eigen/Core"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/controller/LinearQuadraticRegulator.h"
#include "frc/estimator/KalmanFilter.h"
#include "frc/system/LinearSystem.h"
#include "units/time.h"
#include "units/voltage.h"

namespace frc {

/**
 * Combines a controller, feedforward, and observer for controlling a mechanism
 * with full state feedback.
 *
 * For everything in this file, "inputs" and "outputs" are defined from the
 * perspective of the plant. This means U is an input and Y is an output
 * (because you give the plant U (powers) and it gives you back a Y (sensor
 * values). This is the opposite of what they mean from the perspective of the
 * controller (U is an output because that's what goes to the motors and Y is an
 * input because that's what comes back from the sensors).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf.
 */
template <int States, int Inputs, int Outputs>
class LinearSystemLoop {
 public:
  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer. By default, the initial reference is all zeros. Users should
   * call reset with the initial system state before enabling the loop. This
   * constructor assumes that the input(s) to this system are voltage.
   *
   * @param controller State-space controller.
   * @param observer   State-space observer.
   * @param maxVoltage The maximum voltage that can be applied. Commonly 12.
   * @param dt         The nominal timestep.
   */
  LinearSystemLoop(LinearSystem<States, Inputs, Outputs>& plant,
                   LinearQuadraticRegulator<States, Inputs>& controller,
                   KalmanFilter<States, Inputs, Outputs>& observer,
                   units::volt_t maxVoltage, units::second_t dt)
      : LinearSystemLoop(
            plant, controller, observer,
            [=](Eigen::Matrix<double, Inputs, 1> u) {
              return frc::NormalizeInputVector<Inputs>(
                  u, maxVoltage.template to<double>());
            },
            dt) {}

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer. By default, the initial reference is all zeros. Users should
   * call reset with the initial system state before enabling the loop. This
   * constructor assumes that the input(s) to this system are voltage.
   *
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param observer   State-space observer.
   * @param clampFunction The function used to clamp the input vector.
   * @param dt         The nominal timestep.
   */
  LinearSystemLoop(LinearSystem<States, Inputs, Outputs>& plant,
                   LinearQuadraticRegulator<States, Inputs>& controller,
                   KalmanFilter<States, Inputs, Outputs>& observer,
                   std::function<Eigen::Matrix<double, Inputs, 1>(
                       const Eigen::Matrix<double, Inputs, 1>&)>
                       clampFunction,
                   units::second_t dt)
      : LinearSystemLoop(
            controller,
            LinearPlantInversionFeedforward<States, Inputs>{plant, dt},
            observer, clampFunction) {}

  /**
   * Constructs a state-space loop with the given controller, feedforward and
   * observer. By default, the initial reference is all zeros. Users should
   * call reset with the initial system state.
   *
   * @param controller  State-space controller.
   * @param feedforward Plant inversion feedforward.
   * @param observer    State-space observer.
   * @param maxVoltage  The maximum voltage that can be applied. Assumes
   * that the inputs are voltages.
   */
  LinearSystemLoop(
      LinearQuadraticRegulator<States, Inputs>& controller,
      const LinearPlantInversionFeedforward<States, Inputs>& feedforward,
      KalmanFilter<States, Inputs, Outputs>& observer, units::volt_t maxVoltage)
      : LinearSystemLoop(controller, feedforward, observer,
                         [=](Eigen::Matrix<double, Inputs, 1> u) {
                           return frc::NormalizeInputVector<Inputs>(
                               u, maxVoltage.template to<double>());
                         }) {}

  /**
   * Constructs a state-space loop with the given controller, feedforward,
   * observer and clamp function. By default, the initial reference is all
   * zeros. Users should call reset with the initial system state.
   *
   * @param controller    State-space controller.
   * @param feedforward   Plant-inversion feedforward.
   * @param observer      State-space observer.
   * @param clampFunction The function used to clamp the input vector.
   */
  LinearSystemLoop(
      LinearQuadraticRegulator<States, Inputs>& controller,
      const LinearPlantInversionFeedforward<States, Inputs>& feedforward,
      KalmanFilter<States, Inputs, Outputs>& observer,
      std::function<Eigen::Matrix<double, Inputs, 1>(
          const Eigen::Matrix<double, Inputs, 1>&)>
          clampFunction)
      : m_controller(&controller),
        m_feedforward(feedforward),
        m_observer(&observer),
        m_clampFunc(clampFunction) {
    m_nextR.setZero();
    Reset(m_nextR);
  }

  LinearSystemLoop(LinearSystemLoop&&) = default;
  LinearSystemLoop& operator=(LinearSystemLoop&&) = default;

  /**
   * Returns the observer's state estimate x-hat.
   */
  const Eigen::Matrix<double, States, 1>& Xhat() const {
    return m_observer->Xhat();
  }

  /**
   * Returns an element of the observer's state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const { return m_observer->Xhat(i); }

  /**
   * Returns the controller's next reference r.
   */
  const Eigen::Matrix<double, States, 1>& NextR() const { return m_nextR; }

  /**
   * Returns an element of the controller's next reference r.
   *
   * @param i Row of r.
   */
  double NextR(int i) const { return NextR()(i); }

  /**
   * Returns the controller's calculated control input u.
   */
  Eigen::Matrix<double, Inputs, 1> U() const {
    return ClampInput(m_controller->U() + m_feedforward.Uff());
  }

  /**
   * Returns an element of the controller's calculated control input u.
   *
   * @param i Row of u.
   */
  double U(int i) const { return U()(i); }

  /**
   * Set the initial state estimate x-hat.
   *
   * @param xHat The initial state estimate x-hat.
   */
  void SetXhat(const Eigen::Matrix<double, States, 1>& xHat) {
    m_observer->SetXhat(xHat);
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  void SetXhat(int i, double value) { m_observer->SetXhat(i, value); }

  /**
   * Set the next reference r.
   *
   * @param nextR Next reference.
   */
  void SetNextR(const Eigen::Matrix<double, States, 1>& nextR) {
    m_nextR = nextR;
  }

  /**
   * Return the controller used internally.
   */
  const LinearQuadraticRegulator<States, Inputs>& Controller() const {
    return *m_controller;
  }

  /**
   * Return the feedforward used internally.
   *
   * @return the feedforward used internally.
   */
  const LinearPlantInversionFeedforward<States, Inputs> Feedforward() const {
    return m_feedforward;
  }

  /**
   * Return the observer used internally.
   */
  const KalmanFilter<States, Inputs, Outputs>& Observer() const {
    return m_observer;
  }

  /**
   * Zeroes reference r and controller output u. The previous reference
   * of the PlantInversionFeedforward and the initial state estimate of
   * the KalmanFilter are set to the initial state provided.
   *
   * @param initialState The initial state.
   */
  void Reset(Eigen::Matrix<double, States, 1> initialState) {
    m_nextR.setZero();
    m_controller->Reset();
    m_feedforward.Reset(initialState);
    m_observer->SetXhat(initialState);
  }

  /**
   * Returns difference between reference r and current state x-hat.
   */
  const Eigen::Matrix<double, States, 1> Error() const {
    return m_controller->R() - m_observer->Xhat();
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Outputs, 1>& y) {
    m_observer->Correct(U(), y);
  }

  /**
   * Sets new controller output, projects model forward, and runs observer
   * prediction.
   *
   * After calling this, the user should send the elements of u to the
   * actuators.
   *
   * @param dt Timestep for model update.
   */
  void Predict(units::second_t dt) {
    Eigen::Matrix<double, Inputs, 1> u =
        ClampInput(m_controller->Calculate(m_observer->Xhat(), m_nextR) +
                   m_feedforward.Calculate(m_nextR));
    m_observer->Predict(u, dt);
  }

  /**
   * Clamps input vector between system's minimum and maximum allowable input.
   *
   * @param u Input vector to clamp.
   * @return Clamped input vector.
   */
  Eigen::Matrix<double, Inputs, 1> ClampInput(
      const Eigen::Matrix<double, Inputs, 1>& u) const {
    return m_clampFunc(u);
  }

 protected:
  LinearQuadraticRegulator<States, Inputs>* m_controller;
  LinearPlantInversionFeedforward<States, Inputs> m_feedforward;
  KalmanFilter<States, Inputs, Outputs>* m_observer;

  /**
   * Clamping function.
   */
  std::function<Eigen::Matrix<double, Inputs, 1>(
      const Eigen::Matrix<double, Inputs, 1>&)>
      m_clampFunc;

  // Reference to go to in the next cycle (used by feedforward controller).
  Eigen::Matrix<double, States, 1> m_nextR;

  // These are accessible from non-templated subclasses.
  static constexpr int kStates = States;
  static constexpr int kInputs = Inputs;
  static constexpr int kOutputs = Outputs;
};

}  // namespace frc
