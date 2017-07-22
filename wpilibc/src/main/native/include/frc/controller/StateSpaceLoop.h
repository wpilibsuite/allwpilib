/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Eigen/Core>

#include "frc/controller/StateSpaceController.h"
#include "frc/controller/StateSpaceObserver.h"
#include "frc/controller/StateSpacePlant.h"

namespace frc {

/**
 * Combines a plant, controller, and observer for controlling a mechanism with
 * full state feedback.
 *
 * For everything in this file, "inputs" and "outputs" are defined from the
 * perspective of the plant. This means U is an input and Y is an output
 * (because you give the plant U (powers) and it gives you back a Y (sensor
 * values). This is the opposite of what they mean from the perspective of the
 * controller (U is an output because that's what goes to the motors and Y is an
 * input because that's what comes back from the sensors).
 *
 * For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
template <int States, int Inputs, int Outputs>
class StateSpaceLoop {
 public:
  /**
   * A convenience constructor for constructing a state-space loop with the
   * given plant, controller, and observer coefficients.
   *
   * @param plantCoeffs      State-space plant coefficients.
   * @param controllerCoeffs State-space controller coefficients.
   * @param observerCoeffs   State-space observer coefficients.
   */
  StateSpaceLoop(
      const StateSpacePlantCoeffs<States, Inputs, Outputs>& plantCoeffs,
      const StateSpaceControllerCoeffs<States, Inputs, Outputs>&
          controllerCoeffs,
      const StateSpaceObserverCoeffs<States, Inputs, Outputs>& observerCoeffs);

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer.
   *
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param observer   State-space observer.
   */
  StateSpaceLoop(StateSpacePlant<States, Inputs, Outputs>&& plant,
                 StateSpaceController<States, Inputs, Outputs>&& controller,
                 StateSpaceObserver<States, Inputs, Outputs>&& observer);

  virtual ~StateSpaceLoop() = default;

  StateSpaceLoop(StateSpaceLoop&&) = default;
  StateSpaceLoop& operator=(StateSpaceLoop&&) = default;

  /**
   * Enables the controller.
   */
  void Enable();

  /**
   * Disables the controller and zeros the control input.
   */
  void Disable();

  /**
   * Returns the observer's state estimate x-hat.
   */
  const Eigen::Matrix<double, States, 1>& Xhat() const;

  /**
   * Returns an element of the observer's state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  double Xhat(int i) const;

  /**
   * Returns the controller's next reference r.
   */
  const Eigen::Matrix<double, States, 1>& NextR() const;

  /**
   * Returns an element of the controller's next reference r.
   *
   * @param i Row of r.
   */
  double NextR(int i) const;

  /**
   * Returns the controller's calculated control input u.
   */
  const Eigen::Matrix<double, Inputs, 1>& U() const;

  /**
   * Returns an element of the controller's calculated control input u.
   *
   * @param i Row of u.
   */
  double U(int i) const;

  /**
   * Set the initial state estimate x-hat.
   *
   * @param xHat The initial state estimate x-hat.
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
   * Set the next reference r.
   *
   * @param nextR Next reference.
   */
  void SetNextR(const Eigen::Matrix<double, States, 1>& nextR);

  /**
   * Return the plant used internally.
   */
  const StateSpacePlant<States, Inputs, Outputs>& GetPlant() const;

  /**
   * Return the controller used internally.
   */
  const StateSpaceController<States, Inputs, Outputs>& GetController() const;

  /**
   * Return the observer used internally.
   */
  const StateSpaceObserver<States, Inputs, Outputs>& GetObserver() const;

  /**
   * Zeroes reference r, controller output u, plant output y, and state estimate
   * x-hat.
   */
  void Reset();

  /**
   * Returns difference between reference r and x-hat.
   */
  const Eigen::Matrix<double, States, 1> Error() const;

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param y Measurement vector.
   */
  void Correct(const Eigen::Matrix<double, Outputs, 1>& y);

  /**
   * Sets new controller output, projects model forward, and runs observer
   * prediction.
   *
   * After calling this, the user should send the elements of u to the
   * actuators.
   */
  void Predict();

  /**
   * Sets the current controller to be "index". This can be used for gain
   * scheduling.
   *
   * @param index The controller index.
   */
  void SetIndex(int index);

  /**
   * Returns the current controller index.
   */
  int GetIndex() const;

 protected:
  StateSpacePlant<States, Inputs, Outputs> m_plant;
  StateSpaceController<States, Inputs, Outputs> m_controller;
  StateSpaceObserver<States, Inputs, Outputs> m_observer;

  // Reference to go to in the next cycle (used by feedforward controller).
  Eigen::Matrix<double, States, 1> m_nextR;

  // These are accessible from non-templated subclasses.
  static constexpr int kStates = States;
  static constexpr int kInputs = Inputs;
  static constexpr int kOutputs = Outputs;
};

}  // namespace frc

#include "frc/controller/StateSpaceLoop.inc"
