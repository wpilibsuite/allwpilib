/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

@SuppressWarnings({"ClassTypeParameterName", "MemberName"})
public class StateSpaceLoop<States extends Num, Inputs extends Num, Outputs extends Num> {
  // The number of states, inputs, and outputs of this system
  protected final Nat<States> kStates;
  protected final Nat<Inputs> kInputs;
  protected final Nat<Outputs> kOutputs;

  protected StateSpacePlant<States, Inputs, Outputs> m_plant;
  protected StateSpaceController<States, Inputs, Outputs> m_controller;
  protected StateSpaceObserver<States, Inputs, Outputs> m_observer;

  // Reference to go to in the next cycle (used by feedforward controller).
  protected Matrix<States, N1> m_nextR;

  /**
   * A convenience constructor for constructing a state-space loop with the
   * given plant, controller, and observer coefficients.
   *
   * @param plantCoeffs      State-space plant coefficients.
   * @param controllerCoeffs State-space controller coefficients.
   * @param observerCoeffs   State-space observer coefficients.
   */
  public StateSpaceLoop(StateSpacePlantCoeffs<States, Inputs, Outputs> plantCoeffs,
                        StateSpaceControllerCoeffs<States, Inputs, Outputs> controllerCoeffs,
                        StateSpaceObserverCoeffs<States, Inputs, Outputs> observerCoeffs) {
    kStates = plantCoeffs.getStates();
    kInputs = plantCoeffs.getInputs();
    kOutputs = plantCoeffs.getOutputs();

    m_nextR = MatrixUtils.zeros(kStates);

    m_plant = new StateSpacePlant<>(plantCoeffs);
    m_controller = new StateSpaceController<>(controllerCoeffs, m_plant);
    m_observer = new StateSpaceObserver<>(observerCoeffs, m_plant);
  }

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer.
   *
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param observer   State-space observer.
   */
  public StateSpaceLoop(StateSpacePlant<States, Inputs, Outputs> plant,
                        StateSpaceController<States, Inputs, Outputs> controller,
                        StateSpaceObserver<States, Inputs, Outputs> observer) {
    kStates = plant.getStates();
    kInputs = plant.getInputs();
    kOutputs = plant.getOutputs();

    m_nextR = MatrixUtils.zeros(kStates);

    m_plant = plant;
    m_controller = controller;
    m_observer = observer;
  }

  /**
   * Enables the controller.
   */
  public void enable() {
    m_controller.enable();
  }

  /**
   * Disables the controller and zeros the control input.
   */
  public void disable() {
    m_controller.disable();
  }

  /**
   * Returns the observer's state estimate x-hat.
   */
  public Matrix<States, N1> getXhat() {
    return m_observer.getXhat();
  }

  /**
   * Returns an element of the observer's state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  @SuppressWarnings("ParameterName")
  public double getXhat(int i) {
    return m_observer.getXhat(i);
  }

  /**
   * Returns the controller's next reference r.
   */
  public Matrix<States, N1> getNextR() {
    return m_nextR;
  }

  /**
   * Returns an element of the controller's next reference r.
   *
   * @param i Row of r.
   */
  @SuppressWarnings("ParameterName")
  public double getNextR(int i) {
    return m_nextR.get(i, 0);
  }

  /**
   * Returns the controller's calculated control input u.
   */
  public Matrix<Inputs, N1> getU() {
    return m_controller.getU();
  }

  /**
   * Returns an element of the controller's calculated control input u.
   *
   * @param i Row of u.
   */
  @SuppressWarnings("ParameterName")
  public double getU(int i) {
    return m_controller.getU(i);
  }

  /**
   * Set the initial state estimate x-hat.
   *
   * @param xHat The initial state estimate x-hat.
   */
  @SuppressWarnings("ParameterName")
  public void setXhat(Matrix<States, N1> xHat) {
    m_observer.setXhat(xHat);
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  @SuppressWarnings("ParameterName")
  public void setXhat(int i, double value) {
    m_observer.setXhat(i, value);
  }

  /**
   * Set the next reference r.
   *
   * @param nextR Next reference.
   */
  public void setNextR(Matrix<States, N1> nextR) {
    m_nextR = nextR;
  }

  /**
   * Return the plant used internally.
   */
  public StateSpacePlant<States, Inputs, Outputs> getPlant() {
    return m_plant;
  }

  /**
   * Return the controller used internally.
   */
  public StateSpaceController<States, Inputs, Outputs> getController() {
    return m_controller;
  }

  /**
   * Return the observer used internally.
   */
  public StateSpaceObserver<States, Inputs, Outputs> getObserver() {
    return m_observer;
  }

  /**
   * Zeroes reference r, controller output u, plant output y, and state estimate
   * x-hat.
   */
  public void reset() {
    m_plant.reset();
    m_controller.reset();
    m_observer.reset();
    m_nextR = MatrixUtils.zeros(kStates);
  }

  /**
   * Returns difference between reference r and x-hat.
   */
  public Matrix<States, N1> getError() {
    return m_controller.getR().minus(m_observer.getXhat());
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  public void correct(Matrix<Outputs, N1> y) {
    m_observer.correct(m_controller.getU(), y);
  }

  /**
   * Sets new controller output, projects model forward, and runs observer
   * prediction.
   *
   * <p>After calling this, the user should send the elements of u to the
   * actuators.
   */
  public void predict() {
    m_controller.update(m_observer.getXhat(), m_nextR);
    m_observer.predict(m_controller.getU());
  }

  /**
   * Sets the current controller to be "index". This can be used for gain
   * scheduling.
   *
   * @param index The controller index.
   */
  public void setIndex(int index) {
    m_plant.setIndex(index);
    m_controller.setIndex(index);
    m_observer.setIndex(index);
  }

  /**
   * Returns the current controller index.
   */
  public int getIndex() {
    return m_plant.getIndex();
  }

  /**
   * Returns whether this loop is enabled.
   *
   * @return Whether the output of the loop is enabled.
   */
  public boolean isEnabled() {
    return m_controller.isEnabled();
  }
}
